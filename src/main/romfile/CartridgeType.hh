#pragma once

#include <string>

#include "Common.hh"

namespace gb4e
{
enum class CartridgeTypeValue {
    ROM_ONLY,
    MBC1,
    MBC1_RAM,
    MBC1_RAM_BATTERY,
    MBC2,
    MBC2_BATTERY,
    ROM_RAM,
    ROM_RAM_BATTERY,
    MMM01,
    MMM01_RAM,
    MMM01_RAM_BATTERY,
    MBC3_TIMER_BATTERY,
    MBC3_TIMER_RAM_BATTERY,
    MBC3,
    MBC3_RAM,
    MBC3_RAM_BATTERY,
    MBC5,
    MBC5_RAM,
    MBC5_RAM_BATTERY,
    MBC5_RUMBLE,
    MBC5_RUMBLE_RAM,
    MBC5_RUMBLE_RAM_BATTERY,
    MBC6,
    MBC7_SENSOR_RUMBLE_RAM_BATTERY,
    POCKET_CAMERA,
    BANDAI_TAMA5,
    HUC3,
    HUC1_RAM_BATTERY,
    UNKNOWN
};

class CartridgeType
{
public:
    CartridgeType(CartridgeTypeValue type) : type(type) {}

    CartridgeTypeValue GetType() const { return type; }

    bool IsRomOnly() const { return type == CartridgeTypeValue::ROM_ONLY; }
    bool IsMbc1() const
    {
        return type == CartridgeTypeValue::MBC1 || type == CartridgeTypeValue::MBC1_RAM ||
               type == CartridgeTypeValue::MBC1_RAM_BATTERY;
    }
    bool IsMbc2() const { return type == CartridgeTypeValue::MBC2 || type == CartridgeTypeValue::MBC2_BATTERY; }
    bool IsMbc3() const
    {
        return type == CartridgeTypeValue::MBC3 || type == CartridgeTypeValue::MBC3_RAM ||
               type == CartridgeTypeValue::MBC3_RAM_BATTERY || type == CartridgeTypeValue::MBC3_TIMER_BATTERY ||
               type == CartridgeTypeValue::MBC3_TIMER_RAM_BATTERY;
    }
    bool IsMbc5() const
    {
        return type == CartridgeTypeValue::MBC5 || type == CartridgeTypeValue::MBC5_RAM ||
               type == CartridgeTypeValue::MBC5_RAM_BATTERY || type == CartridgeTypeValue::MBC5_RUMBLE ||
               type == CartridgeTypeValue::MBC5_RUMBLE_RAM || type == CartridgeTypeValue::MBC5_RUMBLE_RAM_BATTERY;
    }
    bool IsHuc1() const { return type == CartridgeTypeValue::HUC1_RAM_BATTERY; }

private:
    CartridgeTypeValue type;
};

CartridgeType const * ToCartridgeType(u8 cartridgeTypeByte);
std::string ToString(CartridgeTypeValue cartridgeType);
};
