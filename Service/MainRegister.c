#include "MainRegister.h"
#include <string.h>

Module_Error_Union_t ModuleError;
const uint16_t ModuleErrorList[ERROR_MAX] = {
    (GENERAL << 12) | ERROR_RESERVE,
};

const uint8_t serialNum[11] = "YYMMDDSSSS";
const uint8_t hardwareVer[3] = "HV";
const uint8_t softwareVer[3] = "SV";
const uint8_t softwareTime[13] = "MMDDYYHHMMSS";

uint16_t ModuleReg[MODULE_REG_NUM_MAX];
const uint16_t moduleRegReadOnlyBit[MODULE_REG_NUM_MAX >> 4] = {
    0xFFFE,
    0xFFFF,
    0xDFFF,
    0xFFFF,
};

void MainReg_Initial(void)
{
    /* Lack of recover function from the EEPROM, By Leo */
    memset(ModuleReg, 0x00, sizeof(ModuleReg));
    memcpy(R_SERIAL_NUM_PTR, serialNum, 10);
    memcpy(R_HARDWARE_VER_PTR, hardwareVer, 2);
    memcpy(R_SOFTWARE_VER_PTR, softwareVer, 2);
    memcpy(R_SOFTWARE_TIME_PTR, softwareTime, 12);
    ModuleError.flg = 0;
}

uint16_t* ptr_GetMainReg = NULL;
uint16_t* GetMainReg(uint16_t addr, uint16_t* usable)
{
    if ((addr >= MODULE_REG_BASE_ADDRESS) && (addr < (MODULE_REG_BASE_ADDRESS + MODULE_REG_NUM_MAX))) {
        ptr_GetMainReg = ModuleReg + (addr & 0xFF);
        *usable = 1;
    } else {
        *usable = 0;
    }

    return ptr_GetMainReg;
}

uint16_t WriteMainReg(uint16_t addr, uint16_t val)
{
    uint16_t result = 0;
    uint16_t readOnlyArray = 0;
    uint16_t index = 0;

    if ((addr >= MODULE_REG_BASE_ADDRESS) && (addr < (MODULE_REG_BASE_ADDRESS + MODULE_REG_NUM_MAX))) {
        index = addr - MODULE_REG_BASE_ADDRESS;
        readOnlyArray = moduleRegReadOnlyBit[(index & 0x00F0) >> 4];
        if ((readOnlyArray & (0x01 << ((index & 0x000F)))) == 0) {  // No read only
            if (&ModuleReg[index] == &R_ENABLE) {
                if (val > 0) {
                    ModuleError.bits.reserve = 1;
                } else {
                    ModuleError.bits.reserve = 0;
                    ModuleReg[index] = val;
                }
            } else {
                ModuleReg[index] = val;
            }
        } else {
            result = 1;
        }
    } else {
        result = 1;
    }

    return result;
}
