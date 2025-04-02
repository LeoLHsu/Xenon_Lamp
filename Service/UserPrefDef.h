#ifndef __USERPREFDEF_H__
#define __USERPREFDEF_H__
#include "main.h"
#include <stdint.h>

typedef struct {
    uint32_t lampWorkTimer_Min;
    uint32_t lampLifeTime_Hr;
    uint32_t reserve[1];
    uint32_t CRC16;
} UserSettingType;

typedef struct {
    uint32_t Index;
    UserSettingType User;
    uint32_t Inv_Index;
} EEPromType;                       // aligned to double word

#define FLASH_EMPTY                 (0xFF)
#define EEPROM_BASE                 FLASH_BASE

#define USER_FLASH_SECTOR_NUM       (1)     // 1 OR 2
#if (USER_FLASH_SECTOR_NUM == 1)
#define USER_FLASH_SECTOR_SIZE      FLASH_SECTOR_SIZE
#else
#define USER_FLASH_SECTOR_SIZE      FLASH_SECTOR_SIZE
#endif
#define EEPROM_ADDR_1               (EEPROM_BASE + (FLASH_SECTOR_TOTAL - 1) * FLASH_SECTOR_SIZE)
#define EEPROM_ADDR_2               (EEPROM_BASE + (FLASH_SECTOR_TOTAL - USER_FLASH_SECTOR_NUM) * FLASH_SECTOR_SIZE)
#define EE_DATA_SIZE                sizeof(EEPromType)
#define USER_SETTING_SIZE           sizeof(UserSettingType)
#define NVRAM_ADDR_USER_SIZE        (USER_FLASH_SECTOR_SIZE * USER_FLASH_SECTOR_NUM)
#define NVRAM_ADDR_USER_BASE        (EEPROM_ADDR_2)
#define NVRAM_ADDR_USER_END         (NVRAM_ADDR_USER_BASE + NVRAM_ADDR_USER_SIZE - 1)

#endif