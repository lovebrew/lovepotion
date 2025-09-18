#pragma once

#include <wut.h>

namespace SEEPROM
{
    struct WUT_PACKED BoardConfig
    {
        uint32_t crc32;
        uint16_t size;
        uint16_t version;
        uint16_t author;
        uint16_t boardType;
        uint16_t boardRevision;
        uint16_t bootSource;
        uint16_t ddr3Size;
        uint16_t ddr3Speed;
        uint16_t ppcClockMultiplier;
        uint16_t iopClockMultiplier;
        uint16_t video1080p;
        uint16_t ddr3Vendor;
        uint16_t movPassiveReset;
        uint16_t sysPllSpeed;
        uint16_t sataDevice;
        uint16_t consoleType;
        uint32_t devicePresence;
        WUT_UNKNOWN_BYTES(0x20);
    };
    WUT_CHECK_SIZE(BoardConfig, 0x48);

    struct WUT_PACKED SysProd
    {
        uint32_t version;
        uint32_t eeprom_version;
        uint32_t product_area;
        uint32_t game_region;
        uint32_t ntsc_pal;
        uint16_t wifi_5ghz_country_code;
        uint16_t wifi_5ghz_country_code_revision;
        char code_id[0x08];
        char serial_id[0x10];
        char model_numer[0x10];
    };
    WUT_CHECK_SIZE(SysProd, 0x40);

    struct WUT_PACKED ProdInfo
    {
        WUT_UNKNOWN_BYTES(0x8);
        uint16_t prod_year;
        uint16_t prod_month_day;
        uint16_t prod_hour_minute;
        uint32_t crc32;
    };
    WUT_CHECK_SIZE(ProdInfo, 0x12);

    struct WUT_PACKED BootParams
    {
        uint16_t cpu_flags;
        uint16_t nand_sd_flags;
        uint32_t nand_config;
        uint32_t nand_bank;
        uint32_t crc32;
    };
    WUT_CHECK_SIZE(BootParams, 0x10);

    struct WUT_PACKED Boot1Info
    {
        uint16_t version;
        uint16_t sector;
        WUT_UNKNOWN_BYTES(0x8);
        uint32_t crc32;
    };
    WUT_CHECK_SIZE(Boot1Info, 0x10);

    struct WUT_PACKED Data
    {
        WUT_UNKNOWN_BYTES(0x12);
        uint8_t rng_seed[0x08];
        WUT_UNKNOWN_BYTES(0x6);
        uint32_t ppc_pvr;
        char seeprom_version_name[0x06];
        uint16_t seeprom_version_code;
        uint16_t otp_version_code;
        uint16_t otp_revision_code;
        char otp_version_name[0x08];
        BoardConfig bc;
        uint8_t drive_key[0x10];
        uint8_t factory_key[0x10];
        uint8_t shdd_key[0x10];
        uint8_t usb_key_seed[0x10];
        uint16_t drive_key_status;
        uint16_t usk_key_status;
        uint16_t shdd_key_status;
        WUT_UNKNOWN_BYTES(0x7A);
        SysProd sys_prod;
        ProdInfo prod_info;
        uint16_t marker0;
        WUT_UNKNOWN_BYTES(0xe);
        uint16_t marker1;
        WUT_UNKNOWN_BYTES(0x1c);
        BootParams boot_params;
        Boot1Info boot1_info[2];
        WUT_UNKNOWN_BYTES(0x10);
    };
    WUT_CHECK_SIZE(Data, 0x200);

    int read(uint8_t* out_buffer, uint32_t offset, uint32_t size);
} // namespace SEEPROM
