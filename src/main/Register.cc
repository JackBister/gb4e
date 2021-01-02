#include "Register.hh"

namespace gb4e
{
Register const REGISTERS[13] = {Register(RegisterName::A),
                                Register(RegisterName::B),
                                Register(RegisterName::C),
                                Register(RegisterName::D),
                                Register(RegisterName::E),
                                Register(RegisterName::H),
                                Register(RegisterName::L),
                                Register(RegisterName::SP),
                                Register(RegisterName::PC),
                                Register(RegisterName::AF),
                                Register(RegisterName::BC),
                                Register(RegisterName::DE),
                                Register(RegisterName::HL)};
}
