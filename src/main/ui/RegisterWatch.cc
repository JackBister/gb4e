#include "RegisterWatch.hh"

#include <imgui.h>

#include "GbCpuState.hh"
#include "Register.hh"
#include "UiCommon.hh"

namespace gb4e::ui
{
using namespace gb4e;
void DrawRegisterWatch(GbCpuState const * state)
{
    if (!showRegisterWatch) {
        return;
    }

    if (ImGui::Begin("Registers")) {
        {
            u16 af = state->Get16BitRegisterValue(GetRegister(RegisterName::AF));
            ImGui::Text("AF");
            ImGui::SameLine();
            ImGui::Text("%04x", af);
            u8 a = state->Get8BitRegisterValue(GetRegister(RegisterName::A));
            ImGui::SameLine();
            ImGui::Text("A");
            ImGui::SameLine();
            ImGui::Text("%02x", a);
            u8 f = state->GetFlags();
            ImGui::SameLine();
            ImGui::Text("F");
            ImGui::SameLine();
            ImGui::Text("%02x", f);
        }

        {
            u16 bc = state->Get16BitRegisterValue(GetRegister(RegisterName::BC));
            ImGui::Text("BC");
            ImGui::SameLine();
            ImGui::Text("%04x", bc);
            u8 b = state->Get8BitRegisterValue(GetRegister(RegisterName::B));
            ImGui::SameLine();
            ImGui::Text("B");
            ImGui::SameLine();
            ImGui::Text("%02x", b);
            u8 c = state->Get8BitRegisterValue(GetRegister(RegisterName::C));
            ImGui::SameLine();
            ImGui::Text("C");
            ImGui::SameLine();
            ImGui::Text("%02x", c);
        }

        {
            u16 de = state->Get16BitRegisterValue(GetRegister(RegisterName::DE));
            ImGui::Text("DE");
            ImGui::SameLine();
            ImGui::Text("%04x", de);
            u8 d = state->Get8BitRegisterValue(GetRegister(RegisterName::D));
            ImGui::SameLine();
            ImGui::Text("D");
            ImGui::SameLine();
            ImGui::Text("%02x", d);
            u8 e = state->Get8BitRegisterValue(GetRegister(RegisterName::E));
            ImGui::SameLine();
            ImGui::Text("E");
            ImGui::SameLine();
            ImGui::Text("%02x", e);
        }

        {
            u16 hl = state->Get16BitRegisterValue(GetRegister(RegisterName::HL));
            ImGui::Text("HL");
            ImGui::SameLine();
            ImGui::Text("%04x", hl);
            u8 h = state->Get8BitRegisterValue(GetRegister(RegisterName::H));
            ImGui::SameLine();
            ImGui::Text("H");
            ImGui::SameLine();
            ImGui::Text("%02x", h);
            u8 l = state->Get8BitRegisterValue(GetRegister(RegisterName::L));
            ImGui::SameLine();
            ImGui::Text("L");
            ImGui::SameLine();
            ImGui::Text("%02x", l);
        }

        {
            u16 sp = state->Get16BitRegisterValue(GetRegister(RegisterName::SP));
            ImGui::Text("SP");
            ImGui::SameLine();
            ImGui::Text("%04x", sp);
        }

        {
            u16 pc = state->Get16BitRegisterValue(GetRegister(RegisterName::PC));
            ImGui::Text("PC");
            ImGui::SameLine();
            ImGui::Text("%04x", pc);
        }
    }
    ImGui::End();
}
};
