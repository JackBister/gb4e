#include "CartridgeType.hh"

namespace gb4e
{
u8 const CARTRIDGE_TYPE_ROM_ONLY_VALUE = 0x00;
u8 const CARTRIDGE_TYPE_MBC1_VALUE = 0x01;
u8 const CARTRIDGE_TYPE_MBC1_RAM_VALUE = 0x02;
u8 const CARTRIDGE_TYPE_MBC1_RAM_BATTERY_VALUE = 0x03;
u8 const CARTRIDGE_TYPE_MBC2_VALUE = 0x05;
u8 const CARTRIDGE_TYPE_MBC2_BATTERY_VALUE = 0x06;
u8 const CARTRIDGE_TYPE_ROM_RAM_VALUE = 0x08;
u8 const CARTRIDGE_TYPE_ROM_RAM_BATTERY_VALUE = 0x09;
u8 const CARTRIDGE_TYPE_MMM01_VALUE = 0x0B;
u8 const CARTRIDGE_TYPE_MMM01_RAM_VALUE = 0x0C;
u8 const CARTRIDGE_TYPE_MMM01_RAM_BATTERY_VALUE = 0x0D;
u8 const CARTRIDGE_TYPE_MBC3_TIMER_BATTERY_VALUE = 0x0F;
u8 const CARTRIDGE_TYPE_MBC3_TIMER_RAM_BATTERY_VALUE = 0x10;
u8 const CARTRIDGE_TYPE_MBC3_VALUE = 0x11;
u8 const CARTRIDGE_TYPE_MBC3_RAM_VALUE = 0x12;
u8 const CARTRIDGE_TYPE_MBC3_RAM_BATTERY_VALUE = 0x13;
u8 const CARTRIDGE_TYPE_MBC5_VALUE = 0x19;
u8 const CARTRIDGE_TYPE_MBC5_RAM_VALUE = 0x1A;
u8 const CARTRIDGE_TYPE_MBC5_RAM_BATTERY_VALUE = 0x1B;
u8 const CARTRIDGE_TYPE_MBC5_RUMBLE_VALUE = 0x1C;
u8 const CARTRIDGE_TYPE_MBC5_RUMBLE_RAM_VALUE = 0x1D;
u8 const CARTRIDGE_TYPE_MBC5_RUMBLE_RAM_BATTERY_VALUE = 0x1E;
u8 const CARTRIDGE_TYPE_MBC6_VALUE = 0x20;
u8 const CARTRIDGE_TYPE_MBC7_SENSOR_RUMBLE_RAM_BATTERY_VALUE = 0x22;
u8 const CARTRIDGE_TYPE_POCKET_CAMERA_VALUE = 0xFC;
u8 const CARTRIDGE_TYPE_BANDAI_TAMA5_VALUE = 0xFD;
u8 const CARTRIDGE_TYPE_HUC3_VALUE = 0xFE;
u8 const CARTRIDGE_TYPE_HUC1_RAM_BATTERY_VALUE = 0xFF;

CartridgeType const CARTRIDGE_TYPE_ROM_ONLY(CartridgeTypeValue::ROM_ONLY);
CartridgeType const CARTRIDGE_TYPE_MBC1(CartridgeTypeValue::MBC1);
CartridgeType const CARTRIDGE_TYPE_MBC1_RAM(CartridgeTypeValue::MBC1_RAM);
CartridgeType const CARTRIDGE_TYPE_MBC1_RAM_BATTERY(CartridgeTypeValue::MBC1_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MBC2(CartridgeTypeValue::MBC2);
CartridgeType const CARTRIDGE_TYPE_MBC2_BATTERY(CartridgeTypeValue::MBC2_BATTERY);
CartridgeType const CARTRIDGE_TYPE_ROM_RAM(CartridgeTypeValue::ROM_RAM);
CartridgeType const CARTRIDGE_TYPE_ROM_RAM_BATTERY(CartridgeTypeValue::ROM_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MMM01(CartridgeTypeValue::MMM01);
CartridgeType const CARTRIDGE_TYPE_MMM01_RAM(CartridgeTypeValue::MMM01_RAM);
CartridgeType const CARTRIDGE_TYPE_MMM01_RAM_BATTERY(CartridgeTypeValue::MMM01_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MBC3_TIMER_BATTERY(CartridgeTypeValue::MBC3_TIMER_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MBC3_TIMER_RAM_BATTERY(CartridgeTypeValue::MBC3_TIMER_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MBC3(CartridgeTypeValue::MBC3);
CartridgeType const CARTRIDGE_TYPE_MBC3_RAM(CartridgeTypeValue::MBC3_RAM);
CartridgeType const CARTRIDGE_TYPE_MBC3_RAM_BATTERY(CartridgeTypeValue::MBC3_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MBC5(CartridgeTypeValue::MBC5);
CartridgeType const CARTRIDGE_TYPE_MBC5_RAM(CartridgeTypeValue::MBC5_RAM);
CartridgeType const CARTRIDGE_TYPE_MBC5_RAM_BATTERY(CartridgeTypeValue::MBC5_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MBC5_RUMBLE(CartridgeTypeValue::MBC5_RUMBLE);
CartridgeType const CARTRIDGE_TYPE_MBC5_RUMBLE_RAM(CartridgeTypeValue::MBC5_RUMBLE_RAM);
CartridgeType const CARTRIDGE_TYPE_MBC5_RUMBLE_RAM_BATTERY(CartridgeTypeValue::MBC5_RUMBLE_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_MBC6(CartridgeTypeValue::MBC6);
CartridgeType const CARTRIDGE_TYPE_MBC7_SENSOR_RUMBLE_RAM_BATTERY(CartridgeTypeValue::MBC7_SENSOR_RUMBLE_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_POCKET_CAMERA(CartridgeTypeValue::POCKET_CAMERA);
CartridgeType const CARTRIDGE_TYPE_BANDAI_TAMA5(CartridgeTypeValue::BANDAI_TAMA5);
CartridgeType const CARTRIDGE_TYPE_HUC3(CartridgeTypeValue::HUC3);
CartridgeType const CARTRIDGE_TYPE_HUC1_RAM_BATTERY(CartridgeTypeValue::HUC1_RAM_BATTERY);
CartridgeType const CARTRIDGE_TYPE_UNKNOWN(CartridgeTypeValue::UNKNOWN);

CartridgeType const * ToCartridgeType(u8 cartridgeTypeByte)
{
    switch (cartridgeTypeByte) {
    case CARTRIDGE_TYPE_ROM_ONLY_VALUE:
        return &CARTRIDGE_TYPE_ROM_ONLY;
    case CARTRIDGE_TYPE_MBC1_VALUE:
        return &CARTRIDGE_TYPE_MBC1;
    case CARTRIDGE_TYPE_MBC1_RAM_VALUE:
        return &CARTRIDGE_TYPE_MBC1_RAM;
    case CARTRIDGE_TYPE_MBC1_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC1_RAM_BATTERY;
    case CARTRIDGE_TYPE_MBC2_VALUE:
        return &CARTRIDGE_TYPE_MBC2;
    case CARTRIDGE_TYPE_MBC2_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC2_BATTERY;
    case CARTRIDGE_TYPE_ROM_RAM_VALUE:
        return &CARTRIDGE_TYPE_ROM_RAM;
    case CARTRIDGE_TYPE_ROM_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_ROM_RAM_BATTERY;
    case CARTRIDGE_TYPE_MMM01_VALUE:
        return &CARTRIDGE_TYPE_MMM01;
    case CARTRIDGE_TYPE_MMM01_RAM_VALUE:
        return &CARTRIDGE_TYPE_MMM01_RAM;
    case CARTRIDGE_TYPE_MMM01_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MMM01_RAM_BATTERY;
    case CARTRIDGE_TYPE_MBC3_TIMER_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC3_TIMER_BATTERY;
    case CARTRIDGE_TYPE_MBC3_TIMER_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC3_TIMER_RAM_BATTERY;
    case CARTRIDGE_TYPE_MBC3_VALUE:
        return &CARTRIDGE_TYPE_MBC3;
    case CARTRIDGE_TYPE_MBC3_RAM_VALUE:
        return &CARTRIDGE_TYPE_MBC3_RAM;
    case CARTRIDGE_TYPE_MBC3_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC3_RAM_BATTERY;
    case CARTRIDGE_TYPE_MBC5_VALUE:
        return &CARTRIDGE_TYPE_MBC5;
    case CARTRIDGE_TYPE_MBC5_RAM_VALUE:
        return &CARTRIDGE_TYPE_MBC5_RAM;
    case CARTRIDGE_TYPE_MBC5_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC5_RAM_BATTERY;
    case CARTRIDGE_TYPE_MBC5_RUMBLE_VALUE:
        return &CARTRIDGE_TYPE_MBC5_RUMBLE;
    case CARTRIDGE_TYPE_MBC5_RUMBLE_RAM_VALUE:
        return &CARTRIDGE_TYPE_MBC5_RUMBLE_RAM;
    case CARTRIDGE_TYPE_MBC5_RUMBLE_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC5_RUMBLE_RAM_BATTERY;
    case CARTRIDGE_TYPE_MBC6_VALUE:
        return &CARTRIDGE_TYPE_MBC6;
    case CARTRIDGE_TYPE_MBC7_SENSOR_RUMBLE_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_MBC7_SENSOR_RUMBLE_RAM_BATTERY;
    case CARTRIDGE_TYPE_POCKET_CAMERA_VALUE:
        return &CARTRIDGE_TYPE_POCKET_CAMERA;
    case CARTRIDGE_TYPE_BANDAI_TAMA5_VALUE:
        return &CARTRIDGE_TYPE_BANDAI_TAMA5;
    case CARTRIDGE_TYPE_HUC3_VALUE:
        return &CARTRIDGE_TYPE_HUC3;
    case CARTRIDGE_TYPE_HUC1_RAM_BATTERY_VALUE:
        return &CARTRIDGE_TYPE_HUC1_RAM_BATTERY;
    }
    return &CARTRIDGE_TYPE_UNKNOWN;
}

std::string ToString(CartridgeTypeValue cartridgeType)
{
    switch (cartridgeType) {
    case CartridgeTypeValue::ROM_ONLY:
        return "CartridgeTypeValue::ROM_ONLY";
    case CartridgeTypeValue::MBC1:
        return "CartridgeTypeValue::MBC1";
    case CartridgeTypeValue::MBC1_RAM:
        return "CartridgeTypeValue::MBC1_RAM";
    case CartridgeTypeValue::MBC1_RAM_BATTERY:
        return "CartridgeTypeValue::MBC1_RAM_BATTERY";
    case CartridgeTypeValue::MBC2:
        return "CartridgeTypeValue::MBC2";
    case CartridgeTypeValue::MBC2_BATTERY:
        return "CartridgeTypeValue::MBC2_BATTERY";
    case CartridgeTypeValue::ROM_RAM:
        return "CartridgeTypeValue::ROM_RAM";
    case CartridgeTypeValue::ROM_RAM_BATTERY:
        return "CartridgeTypeValue::ROM_RAM_BATTERY";
    case CartridgeTypeValue::MMM01:
        return "CartridgeTypeValue::MMM01";
    case CartridgeTypeValue::MMM01_RAM:
        return "CartridgeTypeValue::MMM01_RAM";
    case CartridgeTypeValue::MMM01_RAM_BATTERY:
        return "CartridgeTypeValue::MMM01_RAM_BATTERY";
    case CartridgeTypeValue::MBC3_TIMER_BATTERY:
        return "CartridgeTypeValue::MBC3_TIMER_BATTERY";
    case CartridgeTypeValue::MBC3_TIMER_RAM_BATTERY:
        return "CartridgeTypeValue::MBC3_TIMER_RAM_BATTERY";
    case CartridgeTypeValue::MBC3:
        return "CartridgeTypeValue::MBC3";
    case CartridgeTypeValue::MBC3_RAM:
        return "CartridgeTypeValue::MBC3_RAM";
    case CartridgeTypeValue::MBC3_RAM_BATTERY:
        return "CartridgeTypeValue::MBC3_RAM_BATTERY";
    case CartridgeTypeValue::MBC5:
        return "CartridgeTypeValue::MBC5";
    case CartridgeTypeValue::MBC5_RAM:
        return "CartridgeTypeValue::MBC5_RAM";
    case CartridgeTypeValue::MBC5_RAM_BATTERY:
        return "CartridgeTypeValue::MBC5_RAM_BATTERY";
    case CartridgeTypeValue::MBC5_RUMBLE:
        return "CartridgeTypeValue::MBC5_RUMBLE";
    case CartridgeTypeValue::MBC5_RUMBLE_RAM:
        return "CartridgeTypeValue::MBC5_RUMBLE_RAM";
    case CartridgeTypeValue::MBC5_RUMBLE_RAM_BATTERY:
        return "CartridgeTypeValue::MBC5_RUMBLE_RAM_BATTERY";
    case CartridgeTypeValue::MBC6:
        return "CartridgeTypeValue::MBC6";
    case CartridgeTypeValue::MBC7_SENSOR_RUMBLE_RAM_BATTERY:
        return "CartridgeTypeValue::MBC7_SENSOR_RUMBLE_RAM_BATTERY";
    case CartridgeTypeValue::POCKET_CAMERA:
        return "CartridgeTypeValue::POCKET_CAMERA";
    case CartridgeTypeValue::BANDAI_TAMA5:
        return "CartridgeTypeValue::BANDAI_TAMA5";
    case CartridgeTypeValue::HUC3:
        return "CartridgeTypeValue::HUC3";
    case CartridgeTypeValue::HUC1_RAM_BATTERY:
        return "CartridgeTypeValue::HUC1_RAM_BATTERY";
    case CartridgeTypeValue::UNKNOWN:
    default:
        return "CartridgeTypeValue::UNKNOWN";
    }
}
};
