#include "EEPROM.h"
#include "UserPrefDef.h"
#include "iwdg.h"
#include <string.h>

uint8_t EE_EraseSector(uint32_t Address)
{
    HAL_StatusTypeDef Status = HAL_OK;
    FLASH_EraseInitTypeDef pEraseInit = {0};
    uint32_t *SectorError = NULL;
    uint32_t sectorNum = (Address - FLASH_BASE) / FLASH_SECTOR_SIZE;

    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    pEraseInit.Page = sectorNum;
    pEraseInit.NbPages = 1;

    Set_IWDG1_Timeout(0x0FFF);
    __disable_irq();

    HAL_FLASH_Unlock();
    Status = HAL_FLASHEx_Erase(&pEraseInit, SectorError);
    HAL_FLASH_Lock();

    __enable_irq();
    Set_IWDG1_Timeout(100);

    return (Status == HAL_OK) ? 0 : 1;
}

int8_t EE_WriteTbl(uint32_t addr, uint8_t *buffer, uint32_t count)
{
    HAL_StatusTypeDef Status = HAL_OK;
    uint64_t write_buffer;
    uint32_t Write_Count = count / FLASH_NB_8BITWORD_IN_FLASHWORD;

    __disable_irq();
    HAL_FLASH_Unlock();

    for (uint32_t i = 0; (i < Write_Count) && (Status == HAL_OK); i++) {
        memcpy(&write_buffer, &buffer[i * FLASH_NB_8BITWORD_IN_FLASHWORD], FLASH_NB_8BITWORD_IN_FLASHWORD);
        Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i * FLASH_NB_8BITWORD_IN_FLASHWORD, write_buffer);
    }

    HAL_FLASH_Lock();
    __enable_irq();

    return (Status == HAL_OK) ? 0 : -1;
}

void EE_ReadTbl(uint32_t addr, uint8_t *buffer, uint32_t count)
{
    if ((addr % 4) == 0 && ((uint32_t)buffer % 4) == 0) {
        memcpy(buffer, (void*)addr, count);
    } else {
        for (uint32_t i = 0; i < count; i++) {
            buffer[i] = *(volatile uint8_t*)(addr + i);
        }
    }
}
