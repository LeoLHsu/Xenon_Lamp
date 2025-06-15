#include "MainRegister.h"
#include "Xenon_Lamp.h"
#include <string.h>

Module_Error_Union_t ModuleError;
const uint16_t ModuleErrorList[ERROR_MAX] = {
    (CYCLE << 15)   | (GENERAL << 12)   | ERROR_VOLTAGE_24V,
    (CYCLE << 15)   | (GENERAL << 12)   | ERROR_VOLTAGE_5V,
    (ONCE << 15)    | (ADVISE << 12)    | ERROR_BRIGHTNESS_SETTING,
    (ONCE << 15)    | (ADVISE << 12)    | ERROR_FILTER_SETTING,

    (CYCLE << 15)   | (SERIOUS << 12)   | ERROR_LIGHT_LIFE,
    (ONCE << 15)    | (FATAL << 12)     | ERROR_LIGHT_LIFE_SHOTDOWN,
    (CYCLE << 15)   | (SERIOUS << 12)   | ERROR_LIGHT_HAREWARE_LIFE,
    (ONCE << 15)    | (FATAL << 12)     | ERROR_LIGHT_DAMAGE,

    (CYCLE << 15)   | (SERIOUS << 12)   | ERROR_LIGHT_TEMP,
    (ONCE << 15)    | (FATAL << 12)     | ERROR_LIGHT_TEMP_SHOTDOWN,
    (ONCE << 15)    | (SERIOUS << 12)   | ERROR_FILTER_MOTOR,
    (CYCLE << 15)   | (FATAL << 12)     | ERROR_EXTERNAL_OSC,
};

const uint8_t serialNum[11] = "YYMMDDSSSS";
const uint8_t hardwareVer[3] = "HV";
const uint8_t softwareVer[3] = "SV";
const uint8_t softwareTime[13] = "MMDDYYHHMMSS";

uint16_t ModuleReg[MODULE_REG_NUM_MAX];
const uint16_t moduleRegReadOnlyBit[MODULE_REG_NUM_MAX >> 4] = {
    0xFF8C,
    0xFFFF,
    0xDBFF,
    0xFFFF,
};

void MainReg_Initial(void)
{
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
        ptr_GetMainReg = NULL;
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
            if (&ModuleReg[index] == &R_BRIGHTNESS_SET) {
                if (val > R_BRIGHTNESS_SET_MAX) {
                    ModuleError.bits.brightnessSeting = 1;
                } else {
                    ModuleError.bits.brightnessSeting = 0;
                    ModuleReg[index] = val;
                }
            } else if (&ModuleReg[index] == &R_LIGHT_FILTER_SET) {
                if (val > R_FILTER_SET_MAX) {
                    ModuleError.bits.filterSeting = 1;
                } else {
                    ModuleError.bits.filterSeting = 0;
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
