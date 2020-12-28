#include "DestinationCode.hh"

namespace gb4e
{
DestinationCode ToDestinationCode(u8 destinationCodeByte)
{
    if (destinationCodeByte == 0x00) {
        return DestinationCode::JAPAN;
    }
    if (destinationCodeByte == 0x01) {
        return DestinationCode::NOT_JAPAN;
    }
    return DestinationCode::UNKNOWN;
}

std::string ToString(DestinationCode destinationCode)
{
    switch (destinationCode) {
    case DestinationCode::JAPAN:
        return "DestinationCode::JAPAN";
    case DestinationCode::NOT_JAPAN:
        return "DestinationCode::NOT_JAPAN";
    case DestinationCode::UNKNOWN:
    default:
        return "DestinationCode::UNKNOWN";
    }
}
};