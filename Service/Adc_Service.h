#ifndef __ADC_SERVICE_H__
#define __ADC_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "adc.h"

#define VOLTAGE_24V_ADC_VALUE       (uint16_t)(2.182f / 3.3f * (1 << 12))       // 2708
#define VOLTAGE_24V_ADC_VALUE_MAX   (uint16_t)(VOLTAGE_24V_ADC_VALUE * 1.1f)
#define VOLTAGE_24V_ADC_VALUE_MIN   (uint16_t)(VOLTAGE_24V_ADC_VALUE * 0.9f)
#define VOLTAGE_24V_ADC_VALUE_DIF   (uint16_t)((VOLTAGE_24V_ADC_VALUE_MAX - VOLTAGE_24V_ADC_VALUE_MIN) * 0.1f)

#define VOLTAGE_5V_ADC_VALUE        (uint16_t)(2.5f / 3.3f * (1 << 12))         // 3103
#define VOLTAGE_5V_ADC_VALUE_MAX    (uint16_t)(VOLTAGE_5V_ADC_VALUE * 1.1f)
#define VOLTAGE_5V_ADC_VALUE_MIN    (uint16_t)(VOLTAGE_5V_ADC_VALUE * 0.9f)
#define VOLTAGE_5V_ADC_VALUE_DIF    (uint16_t)((VOLTAGE_5V_ADC_VALUE_MAX - VOLTAGE_5V_ADC_VALUE_MIN) * 0.1f)

typedef enum {
    ADC1_RANK_24V = 0,
    ADC1_RANK_5V,
    ADC1_RANK_LMAP_CURRENT,
    ADC1_RANK_LMAP_VOLTAGE,
    ADC1_RANK_MAX,
} ADC1_Rank_t;

extern volatile uint8_t ADC1Ready, ADC1OverRun;
extern uint16_t ADC1FilterResult[ADC1_RANK_MAX];

extern void Adc_Initial(void);
extern void Adc_Handler(void);

#ifdef __cplusplus
}
#endif

#endif

