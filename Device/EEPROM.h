#ifndef __EEPROM_H__
#define __EEPROM_H__
#include <stdint.h>
#include "main.h"

extern uint8_t EE_EraseSector(uint32_t Address);
extern int8_t EE_WriteTbl(uint32_t addr, uint8_t *buffer, uint32_t count);
extern void EE_ReadTbl(uint32_t addr, uint8_t *buffer, uint32_t count);

#endif
