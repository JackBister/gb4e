#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CartridgeType.hh"
#include "CgbFlag.hh"
#include "Common.hh"
#include "DestinationCode.hh"
#include "RamSize.hh"
#include "RomSize.hh"
#include "SgbFlag.hh"

namespace gb4e
{
class RomFile
{
public:
    class Builder
    {
    public:
        Builder & WithTitle(std::string title);
        Builder & WithHasValidLogoBytes(bool hasValidLogoBytes);
        Builder & WithManufacturerCode(std::string manufacturerCode);
        Builder & WithCgbFlag(CgbFlag cgbFlag);
        Builder & WithLicenseeCode(u16 licenseeCode);
        Builder & WithSgbFlag(SgbFlag sgbFlag);
        Builder & WithCartridgeType(CartridgeType const * cartridgeType);
        Builder & WithRomSize(RomSize const * romSize);
        Builder & WithRamSize(RamSize const * ramSize);
        Builder & WithDestinationCode(DestinationCode destinationCode);
        Builder & WithOldLicenseeCode(u8 oldLicenseeCode);
        Builder & WithVersionNumber(u8 versionNumber);
        Builder & WithHeaderChecksum(u8 headerChecksum);
        Builder & WithHasValidHeaderChecksum(bool hasValidHeaderChecksum);
        Builder & WithGlobalChecksum(u16 globalChecksum);
        Builder & WithHasValidGlobalChecksum(bool hasValidGlobalChecksum);
        Builder & WithSize(size_t size);
        Builder & WithData(std::unique_ptr<u8[]> && data);
        RomFile Build();

    private:
        std::string title;
        bool hasValidLogoBytes;
        std::string manufacturerCode;
        CgbFlag cgbFlag;
        u16 licenseeCode;
        SgbFlag sgbFlag;
        CartridgeType const * cartridgeType;
        RomSize const * romSize;
        RamSize const * ramSize;
        DestinationCode destinationCode;
        u8 oldLicenseeCode;
        u8 versionNumber;
        u8 headerChecksum;
        bool hasValidHeaderChecksum;
        u16 globalChecksum;
        bool hasValidGlobalChecksum;
        size_t size;
        std::unique_ptr<u8[]> data;
    };

    static std::optional<RomFile> Create(size_t size, std::unique_ptr<u8[]> && data);

    std::string const & GetTitle() const { return title; }

    CartridgeType const * GetCartridgeType() const { return cartridgeType; }

    size_t const & GetSize() const { return size; }

    u8 const * GetData() const { return data.get(); }

    std::string ToString() const;

private:
    RomFile(std::string title, bool hasValidLogo, std::string manufacturerCode, CgbFlag cgbFlag, u16 licenseeCode,
            SgbFlag sgbFlag, CartridgeType const * cartridgeType, RomSize const * romSize, RamSize const * ramSize,
            DestinationCode destinationCode, u8 oldLicenseeCode, u8 versionNumber, u8 headerChecksum,
            bool hasValidHeaderChecksum, u16 globalChecksum, bool hasValidGlobalChecksum, size_t size,
            std::unique_ptr<u8[]> && data);

    std::string title;
    bool hasValidLogo;
    std::string manufacturerCode;
    CgbFlag cgbFlag;
    u16 licenseeCode;
    SgbFlag sgbFlag;
    CartridgeType const * cartridgeType;
    RomSize const * romSize;
    RamSize const * ramSize;
    DestinationCode destinationCode;
    u8 oldLicenseeCode;
    u8 versionNumber;
    u8 headerChecksum;
    bool hasValidHeaderChecksum;
    u16 globalChecksum;
    bool hasValidGlobalChecksum;
    size_t size;
    std::unique_ptr<u8[]> data;
};
};