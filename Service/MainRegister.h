#ifndef MAINREGISTER_H_
#define MAINREGISTER_H_
#include "main.h"

#define SERIAL_NUM                  "YYMMDDSSSS"
#define HARDWARE_VER                "HV"
#define SOFTWARE_VER                "SV"
#define SOFTWARE_TIME               "MMDDYYHHMMSS"
#ifdef DEBUG
#define SOFTWARE_INSIDE_VER         (0U)
#else
#define SOFTWARE_INSIDE_VER         (10000U)        // xxxxx stands for Vx.xx.xx
#endif

_Static_assert(sizeof(SERIAL_NUM) - 1 == 10, "SERIAL_NUM must be 10 characters long");
_Static_assert(sizeof(HARDWARE_VER) - 1 == 2, "HARDWARE_VER must be 2 characters long");
_Static_assert(sizeof(SOFTWARE_VER) - 1 == 2, "SOFTWARE_VER must be 2 characters long");
_Static_assert(sizeof(SOFTWARE_TIME) - 1 == 12, "SOFTWARE_TIME must be 12 characters long");
_Static_assert(SOFTWARE_INSIDE_VER <= U16_DUMMY, "SOFTWARE_INSIDE_VER must fit in uint16_t");

typedef enum {
    ONCE = 0,
    CYCLE
} Error_ReportType_t;

typedef enum {
    MESSAGE = 1,
    ADVISE,
    GENERAL,
    SERIOUS,
    FATAL,
    ERROR_LEVEL_MAX
} Error_Level_t;

//////////////////////////////////////////////////////////////////////////////
typedef union {
    uint32_t flg;
    struct {
        uint32_t vol24V: 1;
        uint32_t vol5V: 1;
        uint32_t brightnessSeting: 1;
        uint32_t filterSeting: 1;

        uint32_t lightLife: 1;
        uint32_t lightLifeShotdown: 1;
        uint32_t lightHardwareLife: 1;
        uint32_t lightDamage: 1;

        uint32_t lightTemp: 1;
        uint32_t lightTempShotdown: 1;
        uint32_t filterMotor: 1;
        uint32_t externalOsc: 1;
    } bits;
} Module_Error_Union_t;
extern Module_Error_Union_t ModuleError;

typedef enum {
    ERROR_VOLTAGE_24V = 0,
    ERROR_VOLTAGE_5V,
    ERROR_BRIGHTNESS_SETTING,
    ERROR_FILTER_SETTING,

    ERROR_LIGHT_LIFE,
    ERROR_LIGHT_LIFE_SHOTDOWN,
    ERROR_LIGHT_HAREWARE_LIFE,
    ERROR_LIGHT_DAMAGE,

    ERROR_LIGHT_TEMP,
    ERROR_LIGHT_TEMP_SHOTDOWN,
    ERROR_FILTER_MOTOR,
    ERROR_EXTERNAL_OSC,

    ERROR_MAX
} Module_Error_t;
extern const uint16_t ModuleErrorList[];

#define MODULE_REG_BASE_ADDRESS     0x1600
#define MODULE_REG_NUM_MAX          0x40

extern uint16_t ModuleReg[MODULE_REG_NUM_MAX];

#define R_ENABLE                    ModuleReg[0x00]      // 0=关闭，1=启动  R/W
#define XENON_LAMP_ENABLE           BIT(15)
#define R_BRIGHTNESS_SET            ModuleReg[0x01]      // R/W  0 - 100%
#define R_BRIGHTNESS_CURR           ModuleReg[0x02]      // R    0 - 100%
#define R_LIGHT_WORK_HOUR           ModuleReg[0x03]      // R    h
#define R_LIGHT_LIFE_HOUR           ModuleReg[0x04]      // R/W  h
#define R_LIGHT_WORK_HOUR_RESET     ModuleReg[0x05]      // R/W  0=关闭，1=启动
#define R_LIGHT_FILTER_SET          ModuleReg[0x06]      // R/W  0=无滤光片，1=有滤光片
#define R_LIGHT_FILTER_CURR         ModuleReg[0x07]      // R    0=无滤光片，1=有滤光片
#define R_ERROR_REPORT_TIMEBASE     ModuleReg[0x2A]      // R/W  错误周期上报时间基数 秒
#define R_ERROR_LIST_1              ModuleReg[0x2B]      // R    错误列表1
#define R_ERROR_LIST_2              ModuleReg[0x2C]      // R    错误列表2
#define R_FACTORY_CTRL_0            ModuleReg[0x2D]      // R/W  工厂模式负载控制位
#define R_FACTORY                   ModuleReg[0x2E]      // R    工厂模式
#define R_ERROR_INFO                ModuleReg[0x2F]      // R    报错信息
#define R_ERROR_INFO_ADDR           (MODULE_REG_BASE_ADDRESS | 0x002F)
/*
Bit0~Bit11:Code
Bit12~Bit14:Level
Bit15:0-Clean; 1-Write
*/
#define R_SERIAL_NUM_PTR            &ModuleReg[0x30]        // Serial Number   R lenth == 10 Bytes
#define R_HARDWARE_VER_PTR          &ModuleReg[0x35]        // Hardware Version   R lenth == 2 Bytes
#define R_SOFTWARE_VER_PTR          &ModuleReg[0x36]        // Software Version   R lenth == 2 Bytes
#define R_SOFTWARE_TIME_PTR         &ModuleReg[0x37]        // Software Version   R lenth == 12 Bytes
#define R_SOFTWARE_INDISE_VER_PTR   ModuleReg[0x3D]         // Software inside Version   R lenth == 2 Bytes
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
extern void MainReg_Initial(void);
extern uint16_t* GetMainReg(uint16_t addr, uint16_t* usable);
extern uint16_t WriteMainReg(uint16_t addr, uint16_t val);
//////////////////////////////////////////////////////////////////////////////
#endif
