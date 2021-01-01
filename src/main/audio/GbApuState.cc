/**
 * Adapted from https://github.com/baines/MiniGBS and https://github.com/sysprog21/jitboy
 * Copyright (c) 2020 National Cheng Kung University, Taiwan.
 * Copyright (c) 2017 Alex Baines <alex@abaines.me.uk>

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "GbApuState.hh"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <optional>

#include <SDL2/SDL.h>

#include "Common.hh"
#include "logging/Logger.hh"
#include "ui/Metrics.hh"

static auto const logger = Logger::Create("GbApuState");

namespace gb4e
{
float constexpr AUDIO_SAMPLE_RATE = 48000.f;
float const DIFFS[] = {7.5f, 3.75f, 1.5f};
u8 const DUTY_LOOKUP[] = {0x10, 0x30, 0x3C, 0xCF};
u8 const FREQS[] = {8, 16, 32, 48, 64, 80, 96, 112};

struct ChannelCounter final {
    int load;
    bool enabled;
    float counter;
    float inc;
};

struct ChannelVolumeEnvelope final {
    int step;
    bool up;
    float counter;
    float inc;
};

struct ChannelFreqSweep final {
    u16 freq;
    int rate;
    bool up;
    int shift;
    float counter;
    float inc;
};

struct AudioChannel final {
    float Hipass(float sample);
    void SetNoteFreq(float freq);
    bool UpdateFreq(float * pos);
    void UpdateSweep();
    void UpdateVolumeEnvelope();

    bool enabled;
    bool powered;
    bool onLeft;
    bool onRight;
    bool muted;

    int volume;
    int initialEnvelopeVolume;

    u16 freq;
    float freqCounter;
    float freqInc;

    int val;

    struct ChannelCounter len;
    struct ChannelVolumeEnvelope env;
    struct ChannelFreqSweep sweep;

    /* square */
    int duty;
    int dutyCounter;

    /* noise */
    u16 lfsrReg;
    bool lfsrWide;
    int lfsrDiv;

    /* wave */
    u8 sample;

    float capacitor;
};

class AudioPimpl final
{
public:
    AudioPimpl();

    void TickCycle();

    std::optional<u8> ReadMemory(u16 addr) const;

    bool WriteMemory(u16 addr, u8 value);

private:
    static void AudioCallback(void * userdata, u8 * stream, int len);

    void ChanUpdateLen(u8 i);
    void EnableChan(u8 i, bool enable);
    void TriggerChan(u8 i);
    void UpdateLen(AudioChannel * c);
    void UpdateNoise(float * samples, int sampleCount);
    void UpdateRate();
    void UpdateSquare(float * samples, bool isChannel2, int sampleCount);
    void UpdateWave(float * samples, int sampleCount);
    u8 WaveSample(unsigned int pos, unsigned int volume);

    void LockAudioDevice();
    void UnlockAudioDevice();

    std::array<u8, 0x30> memory;

    u8 tac = 0;
    u8 tma = 0;

    std::atomic_uint64_t cycle = 0;
    u64 lastMainThreadCycle = 0;
    std::chrono::high_resolution_clock::time_point lastCallback = std::chrono::high_resolution_clock::now();

    SDL_AudioDeviceID audioDeviceId = 0;

    AudioChannel chans[4];
    float volLeft, volRight;
};

float AudioChannel::Hipass(float sample)
{
    float out = sample - capacitor;
    capacitor = sample - out * 0.996f;
    return out;
}

void AudioChannel::SetNoteFreq(float freq)
{
    freqInc = freq / AUDIO_SAMPLE_RATE;
}

void AudioPimpl::EnableChan(u8 i, bool enable)
{
    chans[i].enabled = enable;

    u8 val = (memory[0xff26 - 0xff10] & 0x80) | (chans[3].enabled << 3) | (chans[2].enabled << 2) |
             (chans[1].enabled << 1) | (chans[0].enabled << 0);

    memory[0xff26 - 0xff10] = val;
}

void AudioChannel::UpdateVolumeEnvelope()
{
    env.counter += env.inc;

    while (env.counter > 1.0f) {
        if (env.step) {
            volume += env.up ? 1 : -1;
            if (volume == 0 || volume == 15) {
                env.inc = 0;
            }
            volume = std::max(0, std::min(15, volume));
        }
        env.counter -= 1.0f;
    }
}

void AudioPimpl::UpdateLen(AudioChannel * c)
{
    if (c->len.enabled) {
        c->len.counter += c->len.inc;
        if (c->len.counter > 1.0f) {
            EnableChan(c - chans, 0);
            c->len.counter = 0.0f;
        }
    }
}

bool AudioChannel::UpdateFreq(float * pos)
{
    float inc = freqInc - *pos;
    freqCounter += inc;

    if (freqCounter > 1.0f) {
        *pos = freqInc - (freqCounter - 1.0f);
        freqCounter = 0.0f;
        return true;
    } else {
        *pos = freqInc;
        return false;
    }
}

void AudioChannel::UpdateSweep()
{
    sweep.counter += sweep.inc;
    while (sweep.counter > 1.0f) {
        if (sweep.shift) {
            u16 inc = (sweep.freq >> sweep.shift);
            if (!sweep.up) {
                inc *= -1;
            }

            freq = sweep.freq + inc;
            if (freq > 2047) {
                enabled = 0;
            } else {
                SetNoteFreq(4194304.0f / (float)((2048 - freq) << 5));
                sweep.freq = freq;
                freqInc *= 8.0f;
            }
        } else if (sweep.rate) {
            enabled = 0;
        }
        sweep.counter -= 1.0f;
    }
}

void AudioPimpl::UpdateSquare(float * samples, bool isChannel2, int sampleCount)
{
    AudioChannel * c = chans + isChannel2;
    if (!c->powered) {
        return;
    }

    c->SetNoteFreq(4194304.0f / (float)((2048 - c->freq) << 5));
    c->freqInc *= 8.0f;

    for (int i = 0; i < sampleCount; i += 2) {
        UpdateLen(c);

        if (c->enabled) {
            c->UpdateVolumeEnvelope();
            if (!isChannel2) {
                c->UpdateSweep();
            }

            float pos = 0.0f;
            float prevPos = 0.0f;
            float sample = 0.0f;

            while (c->UpdateFreq(&pos)) {
                c->dutyCounter = (c->dutyCounter + 1) & 7;
                sample += ((pos - prevPos) / c->freqInc) * (float)c->val;
                c->val = (c->duty & (1 << c->dutyCounter)) ? 1 : -1;
                prevPos = pos;
            }
            sample += ((pos - prevPos) / c->freqInc) * (float)c->val;
            sample = c->Hipass(sample * (c->volume / 15.0f));

            if (!c->muted) {
                samples[i + 0] += sample * 0.25f * c->onLeft * volLeft;
                samples[i + 1] += sample * 0.25f * c->onRight * volRight;
            }
        }
    }
}

u8 AudioPimpl::WaveSample(unsigned int pos, unsigned int volume)
{
    u8 sample = memory[(0xFF30 + pos / 2) - 0xFF10];
    if (pos & 1) {
        sample &= 0xF;
    } else {
        sample >>= 4;
    }
    return volume ? (sample >> (volume - 1)) : 0;
}

void AudioPimpl::UpdateWave(float * samples, int sampleCount)
{
    AudioChannel * c = chans + 2;
    if (!c->powered)
        return;

    float freq = 4194304.0f / (float)((2048 - c->freq) << 5);
    c->SetNoteFreq(freq);

    c->freqInc *= 16.0f;

    for (int i = 0; i < sampleCount; i += 2) {
        UpdateLen(c);

        if (c->enabled) {
            float pos = 0.0f;
            float prevPos = 0.0f;
            float sample = 0.0f;

            c->sample = WaveSample(c->val, c->volume);

            while (c->UpdateFreq(&pos)) {
                c->val = (c->val + 1) & 31;
                sample += ((pos - prevPos) / c->freqInc) * (float)c->sample;
                c->sample = WaveSample(c->val, c->volume);
                prevPos = pos;
            }
            sample += ((pos - prevPos) / c->freqInc) * (float)c->sample;

            if (c->volume > 0) {
                float diff = DIFFS[c->volume - 1];
                sample = c->Hipass((sample - diff) / 7.5f);

                if (!c->muted) {
                    samples[i + 0] += sample * 0.25f * c->onLeft * volLeft;
                    samples[i + 1] += sample * 0.25f * c->onRight * volRight;
                }
            }
        }
    }
}

void AudioPimpl::UpdateNoise(float * samples, int sampleCount)
{
    AudioChannel * c = chans + 3;
    if (!c->powered)
        return;

    float freq = 4194304.f / (float)(FREQS[c->lfsrDiv] << c->freq);
    c->SetNoteFreq(freq);

    if (c->freq >= 14) {
        c->enabled = 0;
    }

    for (int i = 0; i < sampleCount; i += 2) {
        UpdateLen(c);

        if (c->enabled) {
            c->UpdateVolumeEnvelope();

            float pos = 0.0f;
            float prevPos = 0.0f;
            float sample = 0.0f;

            while (c->UpdateFreq(&pos)) {
                c->lfsrReg = (c->lfsrReg << 1) | (c->val == 1);

                if (c->lfsrWide) {
                    c->val = !(((c->lfsrReg >> 14) & 1) ^ ((c->lfsrReg >> 13) & 1)) ? 1 : -1;
                } else {
                    c->val = !(((c->lfsrReg >> 6) & 1) ^ ((c->lfsrReg >> 5) & 1)) ? 1 : -1;
                }
                sample += ((pos - prevPos) / c->freqInc) * c->val;
                prevPos = pos;
            }
            sample += ((pos - prevPos) / c->freqInc) * c->val;
            sample = c->Hipass(sample * (c->volume / 15.0f));

            if (!c->muted) {
                samples[i + 0] += sample * 0.25f * c->onLeft * volLeft;
                samples[i + 1] += sample * 0.25f * c->onRight * volRight;
            }
        }
    }
}

void AudioPimpl::TriggerChan(u8 i)
{
    AudioChannel * c = chans + i;

    EnableChan(i, 1);
    c->volume = c->initialEnvelopeVolume;

    /* volume envelope */
    {
        u8 val = memory[(0xFF12 + (i * 5)) - 0xff10];

        c->env.step = val & 0x07;
        c->env.up = val & 0x08;
        c->env.inc = c->env.step ? (64.0f / (float)c->env.step) / AUDIO_SAMPLE_RATE : 8.0f / AUDIO_SAMPLE_RATE;
        c->env.counter = 0.0f;
    }

    /* freq sweep */
    if (i == 0) {
        u8 val = memory[0xFF10 - 0xff10];

        c->sweep.freq = c->freq;
        c->sweep.rate = (val >> 4) & 0x07;
        c->sweep.up = !(val & 0x08);
        c->sweep.shift = (val & 0x07);
        c->sweep.inc = c->sweep.rate ? (128.0f / (float)(c->sweep.rate)) / AUDIO_SAMPLE_RATE : 0;
        c->sweep.counter = nexttowardf(1.0f, 1.1f);
    }

    if (i == 2) { /* wave */
        c->val = 0;
    } else if (i == 3) { /* noise */
        c->lfsrReg = 0xFFFF;
        c->val = -1;
    }
}

void AudioPimpl::ChanUpdateLen(u8 i)
{
    AudioChannel * c = chans + i;
    int lenMax = i == 2 ? 256 : 64;
    c->len.inc = (256.0f / (float)(lenMax - c->len.load)) / AUDIO_SAMPLE_RATE;
    c->len.counter = 0.0f;
}

bool AudioPimpl::WriteMemory(const u16 addr, const u8 val)
{
    if (addr == 0xFF06) {
        tma = val;
        UpdateRate();
        return true;
    }
    if (addr == 0xFF07) {
        tac = val;
        UpdateRate();
        return true;
    }
    if (addr < 0xFF10 || addr > 0xFF3F) {
        return false;
    }

    /* Find sound channel corresponding to register address. */
    u8 i = (addr - 0xFF10) / 5;

    logger->Infof("Write addr=%04x, val=%02x", addr, val);

    LockAudioDevice();
    bool result = false;
    switch (addr) {
    case 0xFF12:
    case 0xFF17:
    case 0xFF21: {
        chans[i].initialEnvelopeVolume = val >> 4;
        chans[i].powered = val >> 3;

        /* "zombie mode" stuff, needed for Prehistorik Man and similar */
        if (chans[i].powered && chans[i].enabled) {
            if ((chans[i].env.step == 0 && chans[i].env.inc != 0)) {
                if (val & 0x08) {
                    chans[i].volume++;
                } else {
                    chans[i].volume += 2;
                }
            } else {
                // } else if(chans[i].env.step != (val & 0x07)) {
                chans[i].volume = 16 - chans[i].volume;
            }

            chans[i].volume &= 0x0F;
            chans[i].env.step = val & 0x07;
        }
        result = true;
        break;
    }
    case 0xFF1C:
        chans[i].volume = chans[i].initialEnvelopeVolume = (val >> 5) & 0x03;
        result = true;
        break;
    case 0xFF11:
    case 0xFF16:
    case 0xFF20: {
        chans[i].len.load = val & 0x3f;
        chans[i].duty = DUTY_LOOKUP[val >> 6];
        ChanUpdateLen(i);
        result = true;
        break;
    }

    case 0xFF1B:
        chans[i].len.load = val;
        ChanUpdateLen(i);
        result = true;
        break;

    case 0xFF13:
    case 0xFF18:
    case 0xFF1D:
        chans[i].freq &= 0xFF00;
        chans[i].freq |= val;
        result = true;
        break;

    case 0xFF1A:
        chans[i].powered = val & 0x80;
        EnableChan(i, val & 0x80);
        result = true;
        break;

    case 0xFF14:
    case 0xFF19:
    case 0xFF1E:
        chans[i].freq &= 0x00FF;
        chans[i].freq |= ((val & 0x07) << 8);
    case 0xFF23:
        chans[i].len.enabled = val & 0x40;
        if (val & 0x80) {
            TriggerChan(i);
        }

        result = true;
        break;

    case 0xFF22:
        chans[3].freq = val >> 4;
        chans[3].lfsrWide = !(val & 0x08);
        chans[3].lfsrDiv = val & 0x07;
        result = true;
        break;

    case 0xFF24:
        volLeft = ((val >> 4) & 0x07) / 7.0f;
        volRight = (val & 0x07) / 7.0f;
        result = true;
        break;

    case 0xFF25:
        for (int i = 0; i < 4; ++i) {
            chans[i].onLeft = (val >> (4 + i)) & 1;
            chans[i].onRight = (val >> i) & 1;
        }
        result = true;
        break;
    }

    memory[addr - 0xFF10] = val;

    UnlockAudioDevice();
    return result;
}

std::optional<u8> AudioPimpl::ReadMemory(u16 addr) const
{
    if (addr < 0xFF10 || addr > 0xFF3F) {
        return {};
    }
    // TODO:
    return {};
}

void AudioPimpl::AudioCallback(void * userdata, u8 * stream, int len)
{
    float * samples = (float *)stream;

    auto audioPimpl = (AudioPimpl *)userdata;
    auto now = std::chrono::high_resolution_clock::now();
    u64 duration = (now - audioPimpl->lastCallback).count();
    audioPimpl->lastCallback = now;

    memset(stream, 0, len);

    int sampleCount = len / sizeof(float);

    u64 currentMainThreadCycle = audioPimpl->cycle.load();
    if (currentMainThreadCycle == audioPimpl->lastMainThreadCycle) {
        // Do not play audio if main thread is stalled
        return;
    }
    ui::audioCallbackTimeNs = duration;
    audioPimpl->lastMainThreadCycle = currentMainThreadCycle;

    audioPimpl->UpdateSquare(samples, 0, sampleCount);
    audioPimpl->UpdateSquare(samples, 1, sampleCount);
    audioPimpl->UpdateWave(samples, sampleCount);
    audioPimpl->UpdateNoise(samples, sampleCount);
}

GbApuState::GbApuState() : pimpl(new AudioPimpl()) {}
GbApuState::~GbApuState() = default;

void GbApuState::TickCycle()
{
    pimpl->TickCycle();
}

std::optional<u8> GbApuState::ReadMemory(u16 address) const
{
    return pimpl->ReadMemory(address);
}

bool GbApuState::WriteMemory(u16 address, u8 value)
{
    return pimpl->WriteMemory(address, value);
}

AudioPimpl::AudioPimpl()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    memset(chans, 0, sizeof(chans));
    chans[0].val = chans[1].val = -1;

    WriteMemory(0xFF10, 0x80);
    WriteMemory(0xFF11, 0x3F);
    WriteMemory(0xFF14, 0xB8);
    WriteMemory(0xFF15, 0xFF);
    WriteMemory(0xFF16, 0x03F);
    WriteMemory(0xFF19, 0xB8);
    WriteMemory(0xFF1A, 0x7F);
    WriteMemory(0xFF1B, 0xFF);
    WriteMemory(0xFF1C, 0x9F);
    WriteMemory(0xFF1E, 0xB8);
    WriteMemory(0xFF1F, 0xFF);
    WriteMemory(0xFF20, 0xFF);
    WriteMemory(0xFF23, 0xBF);
    WriteMemory(0xFF24, 0x00);
    WriteMemory(0xFF25, 0x00);
    WriteMemory(0xFF26, 0x70);

    UpdateRate();
}

void AudioPimpl::TickCycle()
{
    if (cycle.fetch_add(1) == 0) {
        logger->Infof("Unpausing audio");
        SDL_PauseAudioDevice(audioDeviceId, 0);
    }
}

void AudioPimpl::UpdateRate()
{
    float audioRate = 59.7f;

    if (tac & 0x04) {
        int rates[] = {4096, 262144, 65536, 16384};
        audioRate = rates[tac & 0x03] / (float)(256 - tma);
        if (tac & 0x80)
            audioRate *= 2.0f;
    }

    int newSampleCount = (int)(AUDIO_SAMPLE_RATE / audioRate) * 2;

    // TODO: No idea if this will work in practice
    if (audioDeviceId) {
        SDL_CloseAudioDevice(audioDeviceId);
    }

    SDL_AudioSpec have;
    SDL_AudioSpec want;
    want.freq = AUDIO_SAMPLE_RATE;
    want.format = AUDIO_F32SYS;
    want.channels = 2;
    want.samples = newSampleCount;
    want.callback = AudioPimpl::AudioCallback;
    want.userdata = this;

    logger->Infof("Audio driver: %s", SDL_GetAudioDeviceName(0, 0));

    if ((audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0)) == 0) {
        printf("SDL could not open audio device: %s\n", SDL_GetError());
        exit(1);
    }

    logger->Infof("Want: %d %d %d %d", want.freq, want.format, want.channels, want.samples);
    logger->Infof("Have: %d %d %d %d", have.freq, have.format, have.channels, have.samples);
}

void AudioPimpl::LockAudioDevice()
{
    SDL_LockAudioDevice(audioDeviceId);
}

void AudioPimpl::UnlockAudioDevice()
{
    SDL_UnlockAudioDevice(audioDeviceId);
}
}