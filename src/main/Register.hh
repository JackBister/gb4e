#pragma once

#include <cassert>
#include <string>

#include "Common.hh"

namespace gb4e
{

enum class RegisterName {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    H = 5,
    L = 6,
    SP = 7,
    PC = 8,
    AF = 9,
    BC = 10,
    DE = 11,
    HL = 12
};

class Register
{
public:
    constexpr Register(RegisterName registerName) : registerName(registerName) {}

    constexpr RegisterName GetRegisterName() const { return registerName; }

    constexpr bool Is8Bit() const
    {
        return registerName == RegisterName::A || registerName == RegisterName::B || registerName == RegisterName::C ||
               registerName == RegisterName::D || registerName == RegisterName::E || registerName == RegisterName::H ||
               registerName == RegisterName::L;
    }

    constexpr bool Is16Bit() const { return !Is8Bit(); }

    /**
     * Returns an index into the registers array in GbCpuState
     * If Is8Bit() is true, the registers array should be cast to u8 * before using the index
     */
    constexpr u8 GetIndex() const
    {

        // TODO: Assumes little endian
        switch (registerName) {
        case RegisterName::A:
            return 1;
        case RegisterName::B:
            return 3;
        case RegisterName::C:
            return 2;
        case RegisterName::D:
            return 5;
        case RegisterName::E:
            return 4;
        case RegisterName::H:
            return 7;
        case RegisterName::L:
            return 6;
        case RegisterName::SP:
            return 4;
        case RegisterName::PC:
            return 5;
        case RegisterName::AF:
            return 0;
        case RegisterName::BC:
            return 1;
        case RegisterName::DE:
            return 2;
        case RegisterName::HL:
            return 3;
        }
        assert(false);
        return 0;
    }

    std::string ToString() const
    {
        switch (registerName) {
        case RegisterName::A:
            return "\"A\"";
        case RegisterName::B:
            return "\"B\"";
        case RegisterName::C:
            return "\"C\"";
        case RegisterName::D:
            return "\"D\"";
        case RegisterName::E:
            return "\"E\"";
        case RegisterName::H:
            return "\"H\"";
        case RegisterName::L:
            return "\"L\"";
        case RegisterName::SP:
            return "\"SP\"";
        case RegisterName::PC:
            return "\"PC\"";
        case RegisterName::AF:
            return "\"AF\"";
        case RegisterName::BC:
            return "\"BC\"";
        case RegisterName::DE:
            return "\"DE\"";
        case RegisterName::HL:
            return "\"HL\"";
        }
        return "\"INVALID\"";
    }

private:
    RegisterName registerName;
};

extern Register const REGISTERS[13];

constexpr Register const * GetRegister(RegisterName registerName)
{
    return &REGISTERS[static_cast<int>(registerName)];
}
};
