#pragma once

#include <string>

#include "Common.hh"

namespace gb4e
{

enum class RegisterName { A, B, C, D, E, H, L, SP, PC, AF, BC, DE, HL };

class Register
{
public:
    Register(RegisterName registerName) : registerName(registerName) {}

    RegisterName GetRegisterName() const { return registerName; }

    bool Is8Bit() const
    {
        return registerName == RegisterName::A || registerName == RegisterName::B || registerName == RegisterName::C ||
               registerName == RegisterName::D || registerName == RegisterName::E || registerName == RegisterName::H ||
               registerName == RegisterName::L;
    }

    bool Is16Bit() const { return !Is8Bit(); }

    /**
     * Returns an index into the registers array in GbCpuState
     * If Is8Bit() is true, the registers array should be cast to u8 * before using the index
     */
    u8 GetIndex() const;

    std::string ToString() const;

private:
    RegisterName registerName;
};

Register const * GetRegister(RegisterName registerName);
};
