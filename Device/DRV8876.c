#include "drv8876.h"
#include "Adc_Service.h"
#include <math.h>

DRV8876_Status motor_status = {0};

/**
  * @brief  初始化DRV8876驱动器
  */
void DRV8876_Init(void)
{
    // 唤醒驱动器
    DRV8876_Wake();

    // 初始状态: 停止
    DRV8876_SetStatus(MOTOR_STOP);

    motor_status.direction = MOTOR_STOP;
    motor_status.enabled = 0;
    motor_status.fault = 0;
    motor_status.current = 0.0f;
}

/**
  * @brief  设置电机状态
  * @param  dir: 电机方向(MOTOR_FORWARD, MOTOR_REVERSE, MOTOR_STOP, MOTOR_BRAKE)
  */
void DRV8876_SetStatus(Motor_Direction dir)
{
    switch (dir) {
        case MOTOR_FORWARD:
            HAL_GPIO_WritePin(DRV8876_PH_PORT, DRV8876_PH_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DRV8876_EN_PORT, DRV8876_EN_PIN, GPIO_PIN_SET);
            motor_status.direction = MOTOR_FORWARD;
            motor_status.enabled = 1;
            break;

        case MOTOR_REVERSE:
            HAL_GPIO_WritePin(DRV8876_PH_PORT, DRV8876_PH_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(DRV8876_EN_PORT, DRV8876_EN_PIN, GPIO_PIN_SET);
            motor_status.direction = MOTOR_REVERSE;
            motor_status.enabled = 1;
            break;

        case MOTOR_STOP:
            HAL_GPIO_WritePin(DRV8876_EN_PORT, DRV8876_EN_PIN, GPIO_PIN_RESET);
            motor_status.direction = MOTOR_STOP;
            motor_status.enabled = 0;
            break;

        case MOTOR_BRAKE:
            HAL_GPIO_WritePin(DRV8876_PH_PORT, DRV8876_PH_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(DRV8876_EN_PORT, DRV8876_EN_PIN, GPIO_PIN_SET);
            motor_status.direction = MOTOR_BRAKE;
            motor_status.enabled = 1;
            break;
    }
}

/**
  * @brief  使驱动器进入睡眠模式
  */
void DRV8876_Sleep(void)
{
    HAL_GPIO_WritePin(DRV8876_nSLEEP_PORT, DRV8876_nSLEEP_PIN, GPIO_PIN_RESET);
    motor_status.enabled = 0;
}

/**
  * @brief  唤醒驱动器
  */
void DRV8876_Wake(void)
{
    HAL_GPIO_WritePin(DRV8876_nSLEEP_PORT, DRV8876_nSLEEP_PIN, GPIO_PIN_SET);
}

/**
  * @brief  检查故障状态
  * @retval 0: 正常, 1: 故障
  */
uint8_t DRV8876_CheckFault(void)
{
    uint8_t fault = HAL_GPIO_ReadPin(DRV8876_nFAULT_PORT, DRV8876_nFAULT_PIN) == GPIO_PIN_RESET;
    motor_status.fault = fault;
    return fault;
}

/**
  * @brief  读取当前电流
  * @retval 电流值(安培)
  */
float DRV8876_ReadCurrent(void)
{
    uint32_t adc_value = 0;
    float voltage = 0.0f;
    float current = 0.0f;

    // 计算电流
    adc_value = ADC1FilterResult[ADC1_RANK_MOTOR_CURR];
    voltage = (float)adc_value * ADC_DAC_VEF_VOL / 65535;
    current = voltage / CURRENT_SENSE_GAIN;

    motor_status.current = current;
    return current;
}

/**
  * @brief  获取驱动器状态
  * @retval DRV8876_Status结构体
  */
DRV8876_Status DRV8876_GetStatus(void)
{
    DRV8876_CheckFault();
    DRV8876_ReadCurrent();

    return motor_status;
}