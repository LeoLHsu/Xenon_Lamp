#ifndef __ADC_SERVICE_H__
#define __ADC_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "adc.h"

#define ADC_DAC_VEF_VOL             (3.0f)

#define VOLTAGE_24V_ADC_VALUE       (uint16_t)(2.182f / ADC_DAC_VEF_VOL * (1 << 16))       // 47,666
#define VOLTAGE_24V_ADC_VALUE_MAX   (uint16_t)(VOLTAGE_24V_ADC_VALUE * 1.1f)
#define VOLTAGE_24V_ADC_VALUE_MIN   (uint16_t)(VOLTAGE_24V_ADC_VALUE * 0.9f)
#define VOLTAGE_24V_ADC_VALUE_DIF   (uint16_t)((VOLTAGE_24V_ADC_VALUE_MAX - VOLTAGE_24V_ADC_VALUE_MIN) * 0.1f)

typedef enum {
    ADC1_RANK_24V = 0,
    ADC1_RANK_LMAP_VOL,
    ADC1_RANK_TEMP,
    ADC1_RANK_LMAP_CURR,

    ADC1_RANK_MOTOR_CURR,
    ADC1_RANK_MAX,
} ADC1_Rank_t;

extern volatile uint8_t ADC1Ready, ADC1OverRun;
extern uint16_t ADC1FilterResult[ADC1_RANK_MAX];

extern void Adc_Initial(void);
extern void Adc_Handler(void);

extern float Calculate_Temperature(uint16_t adcValue);

#ifdef __cplusplus
}
#endif

#endif

