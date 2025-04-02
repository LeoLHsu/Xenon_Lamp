#include "UserPref.h"
#include "EEPROM.h"
#include "Xenon_Lamp.h"
#include "MainRegister.h"

const uint16_t CRCTalbe[] = {
    0x0000, 0xCC01, 0xD801, 0x1400,
    0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401,
    0x5000, 0x9C01, 0x8801, 0x4400
};

uint16_t Get_CRC16(uint8_t *Buffer, uint8_t Len)
{
    uint16_t result = 0xFFFF;
    uint8_t i, Data;
    for (i = 0; i < Len; i++) {
        Data = *Buffer++;
        result = CRCTalbe[(Data ^ result) & 0x0f] ^ (result >> 4);
        result = CRCTalbe[((Data >> 4) ^ result) & 0x0f] ^ (result >> 4);
    }
    result = ((result & 0xFF) << 8) | ((result >> 8) & 0xFF);
    return result;
}

EEPromType EEData;
EEPromType EEData_Backup;
uint32_t EEData_Address = 0;

uint32_t PrepareNextVaildAddress(EEPromType *Buffer, uint32_t CurrentAddress, uint16_t Size, uint32_t StartAddress, uint32_t EndAddress)
{
    uint32_t i;
    uint32_t NextEndAddress;
    uint32_t NextAddress =  CurrentAddress;
    uint8_t *BufferData = (uint8_t *)Buffer;
#if (USER_FLASH_SECTOR_NUM == 2)
    uint32_t Sector_Current, Sector_Next, Sector_Detect;
    Sector_Current = (CurrentAddress + Size - 1 - EEPROM_BASE) / FLASH_SECTOR_SIZE;
    Sector_Detect = Sector_Current;
#endif
    do {
        NextAddress = NextAddress + Size;
        NextEndAddress = NextAddress + Size - 1;
#if (USER_FLASH_SECTOR_NUM == 1)
        if (NextEndAddress > EndAddress) {
            NextAddress = StartAddress;
            EE_EraseSector(NextAddress);
            return NextAddress;
        }
#elif (USER_FLASH_SECTOR_NUM == 2)
        if (NextEndAddress > EndAddress) {
            NextAddress = StartAddress;
            NextEndAddress = NextAddress + Size - 1;
        }
        Sector_Next = (NextEndAddress - EEPROM_BASE) / FLASH_SECTOR_SIZE;
        if (Sector_Next != Sector_Detect) {
            NextAddress = Sector_Next * FLASH_SECTOR_SIZE + EEPROM_BASE;
            if (Sector_Next != Sector_Current) {
                EE_EraseSector(NextAddress);
            } else {}
            if (NextAddress < StartAddress) {
                NextAddress = StartAddress;
            }
            Sector_Detect = Sector_Next;
        }
#endif
        EE_ReadTbl(NextAddress, (uint8_t *)BufferData, Size);
        for (i = 0; i < Size; i++) {
            if (BufferData[i] != FLASH_EMPTY) {
                break;
            }
        }
        if (i >= Size) {
            return NextAddress;
        }
    } while (NextAddress != CurrentAddress);
    EE_EraseSector(StartAddress);
    return StartAddress;
}

uint32_t SaveEESetting(EEPromType *Buffer, EEPromType *Buffer_Backup, uint32_t CurrentAddress, uint16_t Size, uint32_t StartAddress, uint32_t EndAddress)
{
    if (memcmp(Buffer_Backup, Buffer, Size) != 0) {
        Buffer->Index++;
        Buffer->Inv_Index = ~Buffer->Index;
        EE_WriteTbl(CurrentAddress, (uint8_t *)Buffer, Size);
        CurrentAddress = PrepareNextVaildAddress(Buffer_Backup, CurrentAddress, Size, StartAddress, EndAddress);
        memcpy(Buffer_Backup, Buffer, Size);
    }
    return CurrentAddress;
}

uint32_t GetNextAddress(uint32_t CurrentAddress, uint32_t Size)
{
    uint32_t Sector_Current, Sector_Next;
    Sector_Current = (CurrentAddress + Size - 1 - EEPROM_BASE) / FLASH_SECTOR_SIZE;
    Sector_Next = (CurrentAddress + Size + Size - 1 - EEPROM_BASE) / FLASH_SECTOR_SIZE;
    if (Sector_Next == Sector_Current) {
        return CurrentAddress + Size;
    } else {
        return Sector_Next * FLASH_SECTOR_SIZE + EEPROM_BASE;
    }
}

uint32_t GetFirstVaildMultiCopyBlock(EEPromType *Buffer, uint32_t Size, uint32_t StartAddress, uint32_t EndAddress)
{
    uint32_t Index_Address = StartAddress;
    while (Index_Address + Size - 1 <= EndAddress) {
        EE_ReadTbl(Index_Address, (uint8_t *)(Buffer), Size);
        if (Buffer->Index == ~Buffer->Inv_Index) {
            return Index_Address;
        }
        Index_Address = GetNextAddress(Index_Address, Size);
    }
    return 0xFFFFFFFF;
}

void Init_UserSetting(void)
{
    UserSettingLampWorkTimerMin = 0;
    UserSettingLampLifeTimeHr = 500;
}

void Update_UserSetting(void)
{
    UserSettingLampWorkTimerMin = XenonLampWorkTimer_Min;
    UserSettingLampLifeTimeHr = R_LIGHT_LIFE_HOUR;
}

void Check_UserSetting(void)
{

}

void SaveUserSetting(void)
{
    uint32_t CRC16;
    CRC16 = Get_CRC16((uint8_t*)&EEData.User, USER_SETTING_SIZE - 4);
    if (EEData.User.CRC16 != CRC16) {
        EEData.User.CRC16 = CRC16;
        // printData("Write Address %lX\n", EEData_Address);
        EEData_Address = SaveEESetting(&EEData, &EEData_Backup, EEData_Address, EE_DATA_SIZE, (uint32_t)NVRAM_ADDR_USER_BASE, (uint32_t)NVRAM_ADDR_USER_END);
#if (USER_FLASH_SECTOR_NUM == 1)
        if (EEData_Address == NVRAM_ADDR_USER_BASE) {
            // printData("Write Address %lX\n", EEData_Address);
            EE_WriteTbl(EEData_Address, (uint8_t *)&EEData, EE_DATA_SIZE);
            EEData_Address += EE_DATA_SIZE;
        }
#endif
    } else {}
}

void EE_Init_Setting(void)
{
    EEData_Backup.Index = 0x00000000;
    EEData.Index = 0xFFFFFFFF;
    Init_UserSetting();
    Check_UserSetting();
    EEData.User.CRC16 = Get_CRC16((uint8_t*)&EEData.User, USER_SETTING_SIZE - 4);
    EEData_Address = NVRAM_ADDR_USER_BASE;
    if (0 == EE_EraseSector(EEData_Address)) {
        EEData_Address = SaveEESetting(&EEData, &EEData_Backup, EEData_Address, EE_DATA_SIZE, NVRAM_ADDR_USER_BASE, NVRAM_ADDR_USER_END);
    }
}

void EE_Read_Setting(void)
{
    uint32_t Last_Vaild_Index_Value;
    uint32_t Last_Vaild_Index_Address;

    Last_Vaild_Index_Address = GetFirstVaildMultiCopyBlock(&EEData, EE_DATA_SIZE, NVRAM_ADDR_USER_BASE, NVRAM_ADDR_USER_END);
    if (Last_Vaild_Index_Address == 0xFFFFFFFF) {
        EE_Init_Setting();
    } else {
        Last_Vaild_Index_Value = EEData.Index;
        EEData_Address = GetNextAddress(Last_Vaild_Index_Address, EE_DATA_SIZE);
        while (EEData_Address + EE_DATA_SIZE - 1 <= NVRAM_ADDR_USER_END) {
            EE_ReadTbl(EEData_Address, (uint8_t *)(&EEData), EE_DATA_SIZE);
            if (EEData.Index == ~EEData.Inv_Index) {
                if (EEData.Index != (Last_Vaild_Index_Value + 1)) {
                    break;
                } else {
                    Last_Vaild_Index_Value = EEData.Index;
                    Last_Vaild_Index_Address = EEData_Address;
                }
            }
            EEData_Address = GetNextAddress(EEData_Address, EE_DATA_SIZE);
        }
        EEData_Address = Last_Vaild_Index_Address;
        // printData("Read Address %lX\n", EEData_Address);
        EE_ReadTbl(EEData_Address, (uint8_t *)(&EEData), EE_DATA_SIZE);
        EEData_Address = PrepareNextVaildAddress(&EEData_Backup, EEData_Address, EE_DATA_SIZE, NVRAM_ADDR_USER_BASE, NVRAM_ADDR_USER_END);
        memcpy(&EEData_Backup, &EEData, EE_DATA_SIZE);

        if (EEData.User.CRC16 != Get_CRC16((uint8_t*)&EEData.User, USER_SETTING_SIZE - 4)) {
            Init_UserSetting();
            SaveUserSetting();
        }
    }
}

void EE_Setting_Handler(void)
{
    if (SYS_TIM_FLAG_1000MS) {
        Update_UserSetting();
        Check_UserSetting();
        SaveUserSetting();
    }
}
