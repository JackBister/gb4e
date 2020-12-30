#pragma once

#include <cassert>
#include <optional>
#include <vector>

#include "Common.hh"
#include "Register.hh"

namespace gb4e
{
class GbCpuState;
class MemoryState;

class FlagSet
{
public:
    FlagSet(u8 previousValue, u8 value) : previousValue(previousValue), value(value) {}

    u8 GetPreviousValue() const { return previousValue; }
    u8 GetValue() const { return value; }
    std::string ToString() const;

private:
    u8 previousValue;
    u8 value;
};

class MemoryWrite
{
public:
    MemoryWrite(u16 location, u8 previousValue, u8 value)
        : location(location), previousValue(previousValue), value(value)
    {
    }

    u16 GetLocation() const { return location; }
    u8 GetPreviousValue() const { return previousValue; }
    u8 GetValue() const { return value; }
    std::string ToString() const;

private:
    u16 location;
    u8 previousValue;
    u8 value;
};

class RegisterWrite
{
public:
    RegisterWrite(Register const * reg, u8 bytePreviousValue, u8 byteValue)
        : reg(reg), bytePreviousValue(bytePreviousValue), byteValue(byteValue)
    {
        assert(reg->Is8Bit());
    }
    RegisterWrite(Register const * reg, u16 wordPreviousValue, u16 wordValue)
        : reg(reg), wordPreviousValue(wordPreviousValue), wordValue(wordValue)
    {
        assert(reg->Is16Bit());
    }

    Register const * GetRegister() const { return reg; }
    u8 GetBytePreviousValue() const
    {
        assert(reg->Is8Bit());
        return bytePreviousValue;
    }
    u16 GetWordPreviousValue() const
    {
        assert(reg->Is16Bit());
        return wordPreviousValue;
    }
    u8 GetByteValue() const
    {
        assert(reg->Is8Bit());
        return byteValue;
    }
    u16 GetWordValue() const
    {
        assert(reg->Is16Bit());
        return wordValue;
    }

    std::string ToString() const;

private:
    Register const * reg;
    union {
        u8 bytePreviousValue;
        u16 wordPreviousValue;
    };
    union {
        u8 byteValue;
        u16 wordValue;
    };
};

class InstructionResult
{
public:
    InstructionResult() : consumedBytes(1), consumedCycles(1) {}
    InstructionResult(u8 consumedBytes, u8 consumedCycles)
        : consumedBytes(consumedBytes), consumedCycles(consumedCycles)
    {
    }
    InstructionResult(FlagSet flagSet, u8 consumedBytes, u8 consumedCycles)
        : flagSet(flagSet), consumedBytes(consumedBytes), consumedCycles(consumedCycles)
    {
    }
    InstructionResult(FlagSet flagSet, RegisterWrite registerWrite, u8 consumedBytes, u8 consumedCycles)
        : flagSet(flagSet), registerWrites({registerWrite}), consumedBytes(consumedBytes),
          consumedCycles(consumedCycles)
    {
        assert(consumedBytes <= 3);
        assert(consumedCycles <= 6);
    }
    InstructionResult(RegisterWrite registerWrite, u8 consumedBytes, u8 consumedCycles)
        : registerWrites({registerWrite}), consumedBytes(consumedBytes), consumedCycles(consumedCycles)
    {
        assert(consumedBytes <= 3);
        assert(consumedCycles <= 6);
    }
    InstructionResult(std::vector<MemoryWrite> memoryWrites, u8 consumedBytes, u8 consumedCycles)
        : memoryWrites(memoryWrites), consumedBytes(consumedBytes), consumedCycles(consumedCycles)
    {
        assert(consumedBytes <= 3);
        assert(consumedCycles <= 6);
    }
    InstructionResult(std::vector<RegisterWrite> registerWrites, u8 consumedBytes, u8 consumedCycles)
        : registerWrites(registerWrites), consumedBytes(consumedBytes), consumedCycles(consumedCycles)
    {
        assert(consumedBytes <= 3);
        assert(consumedCycles <= 6);
    }
    InstructionResult(MemoryWrite memoryWrite, u8 consumedBytes, u8 consumedCycles)
        : memoryWrites({memoryWrite}), consumedBytes(consumedBytes), consumedCycles(consumedCycles)
    {
    }
    InstructionResult(std::vector<MemoryWrite> memoryWrites, RegisterWrite registerWrite, u8 consumedBytes,
                      u8 consumedCycles)
        : memoryWrites(memoryWrites), registerWrites({registerWrite}), consumedBytes(consumedBytes),
          consumedCycles(consumedCycles)
    {
    }
    InstructionResult(MemoryWrite memoryWrite, RegisterWrite registerWrite, u8 consumedBytes, u8 consumedCycles)
        : memoryWrites({memoryWrite}), registerWrites({registerWrite}), consumedBytes(consumedBytes),
          consumedCycles(consumedCycles)
    {
    }
    InstructionResult(std::vector<MemoryWrite> memoryWrites, std::vector<RegisterWrite> registerWrites,
                      u8 consumedBytes, u8 consumedCycles)
        : memoryWrites(memoryWrites), registerWrites(registerWrites), consumedBytes(consumedBytes),
          consumedCycles(consumedCycles)
    {
    }
    InstructionResult(std::optional<FlagSet> flagSet, std::vector<MemoryWrite> memoryWrites,
                      std::vector<RegisterWrite> registerWrites, u8 consumedBytes, u8 consumedCycles)
        : flagSet(flagSet), memoryWrites(memoryWrites), registerWrites(registerWrites), consumedBytes(consumedBytes),
          consumedCycles(consumedCycles)
    {
        assert(consumedBytes <= 3);
        assert(consumedCycles <= 6);
    }

    std::optional<FlagSet> GetFlagSet() const { return flagSet; }
    std::vector<MemoryWrite> const & GetMemoryWrites() const { return memoryWrites; }
    std::vector<RegisterWrite> const & GetRegisterWrites() const { return registerWrites; }
    u8 GetConsumedBytes() const { return consumedBytes; }
    u8 GetConsumedCycles() const { return consumedCycles; }

    std::string ToString() const;

private:
    std::optional<FlagSet> flagSet;
    std::vector<MemoryWrite> memoryWrites;
    std::vector<RegisterWrite> registerWrites;
    u8 consumedBytes;
    u8 consumedCycles;
};

void ApplyInstructionResult(GbCpuState *, MemoryState *, InstructionResult const &);
};
