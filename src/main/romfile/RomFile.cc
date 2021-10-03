#include "RomFile.hh"

#include <sstream>

#include "logging/Logger.hh"

static auto const logger = Logger::Create("RomFile");

size_t const CARTRIDGE_TYPE_OFFSET = 0x147;
size_t const CGB_FLAG_OFFET = 0x143;
size_t const DESTINATION_CODE_OFFSET = 0x14A;
size_t const GLOBAL_CHECKSUM_START_OFFSET = 0x14E;
size_t const HEADER_CHECKSUM_OFFSET = 0x14D;
size_t const LICENSEE_CODE_START_OFFSET = 0x144;
// clang-format off
u8 const LOGO_BYTES[] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
};
// clang-format on
size_t const LOGO_START_OFFSET = 0x104;
size_t const LOGO_LENGTH = 0x30;
static_assert(sizeof(LOGO_BYTES) == LOGO_LENGTH);
size_t const MANUFACTURER_CODE_START_OFFSET = 0x13F;
size_t const MANUFACTURER_CODE_LENGTH = 0x4;
size_t const OLD_LICENSEE_CODE_OFFSET = 0x14B;
size_t const RAM_SIZE_OFFSET = 0x149;
size_t const ROM_SIZE_OFFSET = 0x148;
size_t const SGB_FLAG_OFFSET = 0x146;
size_t const TITLE_START_OFFSET = 0x134;
size_t const TITLE_LENGTH = 0x10;
size_t const VERSION_NUMBER_OFFSET = 0x14C;

namespace gb4e
{
RomFile::Builder & RomFile::Builder::WithTitle(std::string title)
{
    this->title = title;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithHasValidLogoBytes(bool hasValidLogoBytes)
{
    this->hasValidLogoBytes = hasValidLogoBytes;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithManufacturerCode(std::string manufacturerCode)
{
    this->manufacturerCode = manufacturerCode;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithCgbFlag(CgbFlag)
{
    this->cgbFlag = cgbFlag;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithLicenseeCode(u16 licenseeCode)
{
    this->licenseeCode = licenseeCode;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithSgbFlag(SgbFlag sgbFlag)
{
    this->sgbFlag = sgbFlag;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithCartridgeType(CartridgeType const * cartridgeType)
{
    this->cartridgeType = cartridgeType;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithRomSize(RomSize const * romSize)
{
    this->romSize = romSize;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithRamSize(RamSize const * ramSize)
{
    this->ramSize = ramSize;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithDestinationCode(DestinationCode destinationCode)
{
    this->destinationCode = destinationCode;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithOldLicenseeCode(u8 oldLicenseeCode)
{
    this->oldLicenseeCode = oldLicenseeCode;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithVersionNumber(u8 versionNumber)
{
    this->versionNumber = versionNumber;
    return *this;
}
RomFile::Builder & RomFile::Builder::WithHeaderChecksum(u8 headerChecksum)
{
    this->headerChecksum = headerChecksum;
    return *this;
}
RomFile::Builder & RomFile::Builder::WithHasValidHeaderChecksum(bool hasValidHeaderChecksum)
{
    this->hasValidHeaderChecksum = hasValidHeaderChecksum;
    return *this;
}
RomFile::Builder & RomFile::Builder::WithGlobalChecksum(u16 globalChecksum)
{
    this->globalChecksum = globalChecksum;
    return *this;
}
RomFile::Builder & RomFile::Builder::WithHasValidGlobalChecksum(bool hasValidGlobalChecksum)
{
    this->hasValidGlobalChecksum = hasValidGlobalChecksum;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithSize(size_t size)
{
    this->size = size;
    return *this;
}

RomFile::Builder & RomFile::Builder::WithData(std::unique_ptr<u8[]> && data)
{
    this->data = std::move(data);
    return *this;
}

RomFile RomFile::Builder::Build()
{
    return RomFile(title,
                   hasValidLogoBytes,
                   manufacturerCode,
                   cgbFlag,
                   licenseeCode,
                   sgbFlag,
                   cartridgeType,
                   romSize,
                   ramSize,
                   destinationCode,
                   oldLicenseeCode,
                   versionNumber,
                   headerChecksum,
                   hasValidHeaderChecksum,
                   globalChecksum,
                   hasValidGlobalChecksum,
                   size,
                   std::move(data));
}

std::optional<RomFile> RomFile::Create(size_t size, std::unique_ptr<u8[]> && data)
{
    logger->Tracef("RomFile::Create, size=%zu, ptr=%p", size, data.get());

    if (size < 0x15) {
        logger->Errorf("RomFile size must be at least 0x15 to fit the cartridge header");
        return {};
    }

    char const * titleStart = (char const *)&data[TITLE_START_OFFSET];
    size_t titleLength = TITLE_LENGTH;
    for (size_t i = 0; i < TITLE_LENGTH; ++i) {
        if (data[TITLE_START_OFFSET + i] == '\0') {
            titleLength = i;
            break;
        }
    }
    std::string title(titleStart, titleLength);
    logger->Tracef("RomFile::Create, title=%s", title.c_str());

    char const * logoStart = (char const *)&data[LOGO_START_OFFSET];
    bool hasValidLogo = memcmp(LOGO_BYTES, logoStart, LOGO_LENGTH) == 0;
    if (!hasValidLogo) {
        logger->Warnf("Logo bytes do not match");
    }

    char const * manufacturerCodeStart = (char const *)&data[MANUFACTURER_CODE_START_OFFSET];
    size_t manufacturerCodeLength = 0;
    for (size_t i = 0; i < MANUFACTURER_CODE_LENGTH; ++i) {
        if (data[MANUFACTURER_CODE_START_OFFSET + i] == '\0') {
            manufacturerCodeLength = i;
            break;
        }
    }
    std::string manufacturerCode(manufacturerCodeStart, manufacturerCodeLength);

    u8 cgbFlagByte = data[CGB_FLAG_OFFET];
    CgbFlag cgbFlag = ToCgbFlag(cgbFlagByte);
    if (cgbFlag == CgbFlag::UNKNOWN) {
        logger->Warnf("Got UNKNOWN CgbFlag. cgbFlagByte=%x", cgbFlagByte);
    }

    u16 licenseeCode = *((u16 *)&data[LICENSEE_CODE_START_OFFSET]);

    u8 sgbFlagByte = data[SGB_FLAG_OFFSET];
    SgbFlag sgbFlag = ToSgbFlag(sgbFlagByte);
    if (sgbFlag == SgbFlag::UNKNOWN) {
        logger->Warnf("Got UNKNOWN SgbFlag. sgbFlagByte=%x", sgbFlagByte);
    }

    u8 cartridgeTypeByte = data[CARTRIDGE_TYPE_OFFSET];
    CartridgeType const * cartridgeType = ToCartridgeType(cartridgeTypeByte);
    if (cartridgeType->GetType() == CartridgeTypeValue::UNKNOWN) {
        logger->Warnf("Got UNKNOWN CartridgeType. cartridgeTypeByte=%x", cartridgeTypeByte);
    }

    u8 romSizeByte = data[ROM_SIZE_OFFSET];
    RomSize const * romSize = ToRomSize(romSizeByte);
    if (!romSize->IsValid()) {
        logger->Warnf("Got invalid RomSize. romSizeByte=%x", romSizeByte);
    }

    u8 ramSizeByte = data[RAM_SIZE_OFFSET];
    RamSize const * ramSize = ToRamSize(ramSizeByte);
    if (!ramSize->IsValid()) {
        logger->Warnf("Got invalid RamSize. ramSizeByte=%x", ramSizeByte);
    }

    u8 destinationCodeByte = data[DESTINATION_CODE_OFFSET];
    DestinationCode destinationCode = ToDestinationCode(destinationCodeByte);
    if (destinationCode == DestinationCode::UNKNOWN) {
        logger->Warnf("Got UNKNOWN Destinationcode. destinationCodeByte=%x", destinationCodeByte);
    }

    u8 oldLicenseeCode = data[OLD_LICENSEE_CODE_OFFSET];
    u8 versionNumber = data[VERSION_NUMBER_OFFSET];

    u8 headerChecksum = data[HEADER_CHECKSUM_OFFSET];
    logger->Tracef("Before checking header checksum");
    u16 runningHeaderChecksum = 0;
    for (u16 i = 0x134; i < 0x14D; ++i) {
        runningHeaderChecksum = runningHeaderChecksum - data[i] - 1;
    }
    bool hasValidHeaderChecksum = (runningHeaderChecksum & 0xFF) == headerChecksum;
    if (!hasValidHeaderChecksum) {
        logger->Errorf("Got invalid header checksum. headerChecksum=%x, runningHeaderChecksum=%x",
                       headerChecksum,
                       runningHeaderChecksum & 0xFF);
    }
    logger->Tracef("After checking header checksum");

    u16 globalChecksum = *((u16 *)&data[GLOBAL_CHECKSUM_START_OFFSET]);
    logger->Warnf("TODO: globalChecksum check is currently stubbed");
    bool hasValidGlobalChecksum = true; // TODO:

    return RomFile::Builder()
        .WithTitle(title)
        .WithHasValidLogoBytes(hasValidLogo)
        .WithManufacturerCode(manufacturerCode)
        .WithCgbFlag(cgbFlag)
        .WithLicenseeCode(licenseeCode)
        .WithSgbFlag(sgbFlag)
        .WithCartridgeType(cartridgeType)
        .WithRomSize(romSize)
        .WithRamSize(ramSize)
        .WithDestinationCode(destinationCode)
        .WithOldLicenseeCode(oldLicenseeCode)
        .WithVersionNumber(versionNumber)
        .WithHeaderChecksum(headerChecksum)
        .WithHasValidHeaderChecksum(hasValidHeaderChecksum)
        .WithGlobalChecksum(globalChecksum)
        .WithHasValidGlobalChecksum(hasValidGlobalChecksum)
        .WithSize(size)
        .WithData(std::move(data))
        .Build();
}

RomFile::RomFile(std::string title, bool hasValidLogo, std::string manufacturerCode, CgbFlag cgbFlag, u16 licenseeCode,
                 SgbFlag sgbFlag, CartridgeType const * cartridgeType, RomSize const * romSize, RamSize const * ramSize,
                 DestinationCode destinationCode, u8 oldLicenseeCode, u8 versionNumber, u8 headerChecksum,
                 bool hasValidHeaderChecksum, u16 globalChecksum, bool hasValidGlobalChecksum, size_t size,
                 std::unique_ptr<u8[]> && data)
    : title(title), hasValidLogo(hasValidLogo), manufacturerCode(manufacturerCode), cgbFlag(cgbFlag),
      licenseeCode(licenseeCode), sgbFlag(sgbFlag), cartridgeType(cartridgeType), romSize(romSize), ramSize(ramSize),
      destinationCode(destinationCode), oldLicenseeCode(oldLicenseeCode), versionNumber(versionNumber),
      headerChecksum(headerChecksum), hasValidHeaderChecksum(hasValidHeaderChecksum), globalChecksum(globalChecksum),
      hasValidGlobalChecksum(hasValidGlobalChecksum), size(size), data(std::move(data))
{
}

std::string RomFile::ToString() const
{
    std::stringstream ss;
    ss << '{' << std::boolalpha;
    ss << '\t' << "\"title\": \"" << title << "\",";
    ss << '\t' << "\"hasValidLogo\":" << hasValidLogo << ',';
    ss << '\t' << "\"manufacturerCode\": \"" << manufacturerCode << "\",";
    ss << '\t' << "\"cgbFlag\": \"" << gb4e::ToString(cgbFlag) << "\",";
    ss << '\t' << "\"licenseeCode\":" << std::hex << (int)licenseeCode << ',';
    ss << '\t' << "\"sgbFlag\": \"" << gb4e::ToString(sgbFlag) << "\",";
    ss << '\t' << "\"cartridgeType\": \"" << gb4e::ToString(cartridgeType->GetType()) << "\",";
    ss << '\t' << "\"romSize\": " << romSize->ToString() << ',';
    ss << '\t' << "\"ramSize\": " << ramSize->ToString() << ',';
    ss << '\t' << "\"destinationCode\": \"" << gb4e::ToString(destinationCode) << "\",";
    ss << '\t' << "\"oldLicenseeCode\":" << std::hex << (int)oldLicenseeCode << ',';
    ss << '\t' << "\"versionNumber\": " << std::hex << (int)versionNumber << ',';
    ss << '\t' << "\"headerChecksum\": " << std::hex << (int)headerChecksum << ',';
    ss << '\t' << "\"hasValidHeaderChecksum\": " << hasValidHeaderChecksum << ',';
    ss << '\t' << "\"globalChecksum\": " << std::hex << (int)globalChecksum << ',';
    ss << '\t' << "\"hasValidGlobalChecksum\": " << hasValidGlobalChecksum << ',';
    ss << '\t' << "\"size\": " << std::dec << size << ',';
    ss << '\t' << "\"data\": " << std::hex << (uintptr_t)data.get();
    ss << '}';
    return ss.str();
}
};