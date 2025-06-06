#ifndef __DRV8876_H__
#define __DRV8876_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 引脚配置 - 根据实际硬件连接修改这些宏 */
#define DRV8876_PH_PORT        MOTOR_PH_GPIO_Port
#define DRV8876_PH_PIN         MOTOR_PH_Pin
#define DRV8876_EN_PORT        MOTOR_EN_GPIO_Port
#define DRV8876_EN_PIN         MOTOR_EN_Pin
#define DRV8876_nFAULT_PORT    MOTOR_nFAULT_GPIO_Port
#define DRV8876_nFAULT_PIN     MOTOR_nFAULT_Pin
#define DRV8876_nSLEEP_PORT    MOTOR_nSLEEP_GPIO_Port
#define DRV8876_nSLEEP_PIN     MOTOR_nSLEEP_Pin

/* ADC参数配置 - 用于电流检测 */
#define CURRENT_SENSE_GAIN     1.0f    // 电流检测增益(V/A)
#define MAX_ALLOWED_CURRENT    0.2f    // 最大允许电流(A)

/* 电机控制模式 */
typedef enum {
    MOTOR_STOP = 0,     // 停止
    MOTOR_FORWARD,      // 正向
    MOTOR_REVERSE,      // 反向
    MOTOR_BRAKE         // 刹车
} Motor_Direction;

/* 驱动器状态 */
typedef struct {
    Motor_Direction direction;
    uint8_t enabled;
    uint8_t fault;
    float current;      // 当前电流(A)
} DRV8876_Status;

extern DRV8876_Status motor_status;

/* 函数原型 */
void DRV8876_Init(void);
void DRV8876_SetStatus(Motor_Direction dir);
void DRV8876_Sleep(void);
void DRV8876_Wake(void);
uint8_t DRV8876_CheckFault(void);
float DRV8876_ReadCurrent(void);
DRV8876_Status DRV8876_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV8876_H__ */