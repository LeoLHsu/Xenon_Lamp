#include "EEPROM.h"
#include "UserPrefDef.h"
#include "iwdg.h"
#include <string.h>

uint8_t EE_EraseSector(uint32_t Address)
{
    HAL_StatusTypeDef Status = HAL_OK;
    FLASH_EraseInitTypeDef pEraseInit = {0};
    uint32_t *SectorError = NULL;

    pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
#ifdef DUAL_BANK
    pEraseInit.Banks = FLASH_BANK_2;
    pEraseInit.Sector = (Address - FLASH_BANK2_BASE) / FLASH_SECTOR_SIZE;
#else
    pEraseInit.Banks = FLASH_BANK_1;
    pEraseInit.Sector = (Address - FLASH_BANK1_BASE) / FLASH_SECTOR_SIZE;
#endif
    pEraseInit.NbSectors = 1;
    pEraseInit.VoltageRange = 0x10;

    Set_IWDG1_Timeout(0x0FFF);
    __disable_irq();

    HAL_FLASH_Unlock();
    Status = HAL_FLASHEx_Erase(&pEraseInit, SectorError);
    HAL_FLASH_Lock();

    __enable_irq();
    Set_IWDG1_Timeout(100);

    if (HAL_OK == Status) {
        return 0;
    } else {
        return 1;
    }
}

int8_t EE_WriteTbl(uint32_t addr, uint8_t *buffer, uint32_t count)
{
    HAL_StatusTypeDef Status = HAL_OK;
    int8_t Result = 0;
    uint32_t Word_Buffer[FLASH_NB_32BITWORD_IN_FLASHWORD];
    uint32_t Write_Count = count / (FLASH_NB_32BITWORD_IN_FLASHWORD * 4);
    uint32_t i;

    __disable_irq();
    HAL_FLASH_Unlock();

    for (i = 0; (i < Write_Count) && (Status == HAL_OK); i++) {
        memcpy((uint8_t *)Word_Buffer, &buffer[i * (FLASH_NB_32BITWORD_IN_FLASHWORD * 4)], (FLASH_NB_32BITWORD_IN_FLASHWORD * 4));
        Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr + i * (FLASH_NB_32BITWORD_IN_FLASHWORD * 4), (uint32_t)Word_Buffer);
    }
    if (Status != HAL_OK) {
        Result = -1;
    }

    HAL_FLASH_Lock();
    __enable_irq();

    return Result;
}

void EE_ReadTbl(uint32_t addr, uint8_t *buffer, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++) {
        buffer[i] = *((uint8_t *)(addr + i));
    }
}
