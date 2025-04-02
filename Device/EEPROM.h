#ifndef __EEPROM_H__
#define __EEPROM_H__
#include <stdint.h>
#include "main.h"

// FLASH_SIZE
#define FLASH_SECTOR_SIZE                       2048  // G4系列扇区固定2KB
#define FLASH_SECTOR_TOTAL                      (64)
#define FLASH_NB_8BITWORD_IN_FLASHWORD          8     // 双字编程单位（64-bit）

extern uint8_t EE_EraseSector(uint32_t Address);
extern int8_t EE_WriteTbl(uint32_t addr, uint8_t *buffer, uint32_t count);
extern void EE_ReadTbl(uint32_t addr, uint8_t *buffer, uint32_t count);

#endif
