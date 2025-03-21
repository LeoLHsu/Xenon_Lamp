#ifndef MAINREGISTER_H_
#define MAINREGISTER_H_
#include "main.h"

typedef enum {
    CLEAN = 0,
    REPORT
} Error_Operate_t;

typedef enum {
    MESSAGE = 1,
    ADVISE,
    GENERAL,
    SERIOUS,
    FATAL
} Error_Level_t;

//////////////////////////////////////////////////////////////////////////////
typedef union {
    uint32_t flg;
    struct {
        uint32_t reserve: 32;
    } bits;
} Module_Error_Union_t;
extern Module_Error_Union_t ModuleError;

typedef enum {
    ERROR_RESERVE = 0,
    ERROR_MAX
} Module_Error_t;
extern const uint16_t ModuleErrorList[];

#define MODULE_REG_BASE_ADDRESS  0x0100
#define MODULE_REG_NUM_MAX       0x40

extern uint16_t ModuleReg[MODULE_REG_NUM_MAX];

#define R_ENABLE            ModuleReg[0x00]     // 0=关闭，1=启动  R/W
/*
Bit15:模块使能位
*/
#define R_FACTORY_CTRL_0    ModuleReg[0x2D]     // 工厂模式负载控制位  R/W
#define R_FACTORY           ModuleReg[0x2E]     // 工厂模式 R
#define R_ERROR_INFO        ModuleReg[0x2F]     // 报错信息 R
#define R_ERROR_INFO_ADDR   (MODULE_REG_BASE_ADDRESS | 0x002F)
/*
Bit0~Bit11:Code
Bit12~Bit14:Level
Bit15:0-Clean; 1-Write
*/
#define R_SERIAL_NUM_PTR    &ModuleReg[0x30]        // Serial Number   R lenth == 10 Bytes
#define R_HARDWARE_VER_PTR  &ModuleReg[0x35]        // Hardware Version   R lenth == 2 Bytes
#define R_SOFTWARE_VER_PTR  &ModuleReg[0x36]        // Software Version   R lenth == 2 Bytes
#define R_SOFTWARE_TIME_PTR &ModuleReg[0x37]        // Software Version   R lenth == 12 Bytes
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
extern void MainReg_Initial(void);
extern uint16_t* GetMainReg(uint16_t addr, uint16_t* usable);
extern uint16_t WriteMainReg(uint16_t addr, uint16_t val);
//////////////////////////////////////////////////////////////////////////////
#endif
