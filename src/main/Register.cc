#include "Register.hh"

#include <cassert>

namespace gb4e
{

u8 Register::GetIndex() const
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

std::string Register::ToString() const
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

Register const * GetRegister(RegisterName registerName)
{
    static Register REGISTER_A(RegisterName::A);
    static Register REGISTER_B(RegisterName::B);
    static Register REGISTER_C(RegisterName::C);
    static Register REGISTER_D(RegisterName::D);
    static Register REGISTER_E(RegisterName::E);
    static Register REGISTER_H(RegisterName::H);
    static Register REGISTER_L(RegisterName::L);
    static Register REGISTER_SP(RegisterName::SP);
    static Register REGISTER_PC(RegisterName::PC);
    static Register REGISTER_AF(RegisterName::AF);
    static Register REGISTER_BC(RegisterName::BC);
    static Register REGISTER_DE(RegisterName::DE);
    static Register REGISTER_HL(RegisterName::HL);
    switch (registerName) {
    case RegisterName::A:
        return &REGISTER_A;
    case RegisterName::B:
        return &REGISTER_B;
    case RegisterName::C:
        return &REGISTER_C;
    case RegisterName::D:
        return &REGISTER_D;
    case RegisterName::E:
        return &REGISTER_E;
    case RegisterName::H:
        return &REGISTER_H;
    case RegisterName::L:
        return &REGISTER_L;
    case RegisterName::SP:
        return &REGISTER_SP;
    case RegisterName::PC:
        return &REGISTER_PC;
    case RegisterName::AF:
        return &REGISTER_AF;
    case RegisterName::BC:
        return &REGISTER_BC;
    case RegisterName::DE:
        return &REGISTER_DE;
    case RegisterName::HL:
        return &REGISTER_HL;
    }
    assert(false);
    return nullptr;
}
};