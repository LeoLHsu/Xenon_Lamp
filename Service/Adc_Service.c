#include "Adc_Service.h"
#include "MainRegister.h"
#include <string.h>
#include <stdio.h>

#define FILTER_LOG_TWO  (6)
#define FILTER_CNT      (0x01 << FILTER_LOG_TWO)

uint8_t ADC1FirstOrderFilterArrayPos;
uint8_t ADC1FirstOrderFilterArrayInitFlg;
uint16_t ADC1FirstOrderFilterArray[ADC1_RANK_MAX][FILTER_CNT];
uint16_t ADC1MedianFilterArray[ADC1_RANK_MAX][ADC_BUFFER_CH_SIZE];

volatile uint8_t ADC1Ready, ADC1OverRun;
uint16_t ADC1FilterResult[ADC1_RANK_MAX];

typedef struct {
    float temp;
    float resist;
} TempResistPair;

const TempResistPair B3950_10k_Table[] = {
    {-30, 167200.0f},
    {-25, 127500.0f},
    {-20, 96190.0f},
    {-15, 73560.0f},
    {-10, 56760.0f},
    {-5,  44200.0f},
    {0,   34390.0f},
    {5,   27010.0f},
    {10,  21360.0f},
    {15,  17020.0f},
    {20,  13590.0f},
    {25,  10000.0f},
    {30,  7435.0f},
    {35,  5573.0f},
    {40,  5390.0f},
    {45,  4246.0f},
    {50,  3984.0f},
    {55,  3151.0f},
    {60,  2991.0f},
    {65,  2385.0f},
    {70,  2278.0f},
    {75,  1831.0f},
    {80,  1758.0f},
    {85,  1424.0f},
    {90,  1372.0f},
    {95,  1119.0f},
    {100, 1083.0f},
    {105, 889.7f},
    {110, 863.3f},
    {115, 713.5f},
    {120, 693.7f},
    {125, 576.5f},
    {130, 561.6f},
};

#define TEMP_TABLE_SIZE (sizeof(B3950_10k_Table) / sizeof(B3950_10k_Table[0]))

float Calculate_Temperature(uint16_t adcValue)
{
    float voltage = (adcValue / 65535.0f) * ADC_DAC_VEF_VOL;
    if (voltage <= 0.001f) {
        return B3950_10k_Table[0].temp;
    } else if (voltage >= ADC_DAC_VEF_VOL - 0.001f) {
        return B3950_10k_Table[TEMP_TABLE_SIZE - 1].temp;
    }

    float ntcResistance = ((3.3f - voltage) * 4.7e3f) / voltage;        // 3.3V power supply, 4.7K pull-down resistor
    if (ntcResistance >= B3950_10k_Table[0].resist) {
        return B3950_10k_Table[0].temp;
    } else if (ntcResistance <= B3950_10k_Table[TEMP_TABLE_SIZE - 1].resist) {
        return B3950_10k_Table[TEMP_TABLE_SIZE - 1].temp;
    }

    uint16_t i;
    for (i = 0; i < TEMP_TABLE_SIZE - 1; i++) {
        if ((ntcResistance <= B3950_10k_Table[i].resist) && (ntcResistance >= B3950_10k_Table[i + 1].resist)) {
            break;
        }
    }

    float x0 = B3950_10k_Table[i].resist;
    float x1 = B3950_10k_Table[i + 1].resist;
    float y0 = B3950_10k_Table[i].temp;
    float y1 = B3950_10k_Table[i + 1].temp;

    return y0 + (x0 - ntcResistance) * (y1 - y0) / (x0 - x1);
}

uint16_t Calculate_Average(const uint16_t* array, uint32_t num)
{
    uint32_t sum = 0;

    for (uint32_t index = 0; index < num; ++index) {
        sum += array[index];
    }

    return (uint16_t)((sum + num / 2) / num);
}

void Adc_Initial(void)
{
    ADC1Ready = 0;
    ADC1OverRun = 0;
    ADC1FirstOrderFilterArrayPos = 0;
    ADC1FirstOrderFilterArrayInitFlg = 0;

    memset(ADC1FirstOrderFilterArray, 0x00, sizeof(ADC1FirstOrderFilterArray));
    memset(ADC1FilterResult, 0x00, sizeof(ADC1FilterResult));
}

void Adc1_Get_Result(void)
{
    uint16_t i, j, k;
    uint32_t filterResult[ADC1_RANK_MAX];

    /* Mean filtering */
    memset(filterResult, 0x00, sizeof(filterResult));
    memset(ADC1MedianFilterArray, 0x00, sizeof(ADC1MedianFilterArray));
    for (i = 0; i < ADC1_RANK_MAX; i++) {
        for (j = i, k = 0; j < ADC1_BUFFER_SIZE; j += ADC1_RANK_MAX, k++) {
            ADC1MedianFilterArray[i][k] = ADC1_DATA[j];
        }
        filterResult[i] = Calculate_Average(&ADC1MedianFilterArray[i][0], ADC_BUFFER_CH_SIZE);
    }

    /* First order filtering */
    if (ADC1FirstOrderFilterArrayInitFlg == 0) {
        ADC1FirstOrderFilterArrayInitFlg = 1;
        for (i = 0; i < ADC1_RANK_MAX; i++) {
            for (j = 0; j < FILTER_CNT; j++) {
                ADC1FirstOrderFilterArray[i][j] = filterResult[i];
            }
            ADC1FilterResult[i] = filterResult[i];
            // printData("ADC1FilterResult[%d] = %d\n", i, ADC1FilterResult[i]);
        }
    } else {
        for (i = 0; i < ADC1_RANK_MAX; i++) {
            ADC1FirstOrderFilterArray[i][ADC1FirstOrderFilterArrayPos] = filterResult[i];
        }
        ADC1FirstOrderFilterArrayPos++;
        ADC1FirstOrderFilterArrayPos &= (FILTER_CNT - 1);

        memset(filterResult, 0x00, sizeof(filterResult));
        for (i = 0; i < ADC1_RANK_MAX; i++) {
            for (j = 0; j < FILTER_CNT; j++) {
                filterResult[i] += ADC1FirstOrderFilterArray[i][j];
            }
            ADC1FilterResult[i] = filterResult[i] >> FILTER_LOG_TWO;
            // printData("ADC1FilterResult[%d] = %d\n", i, ADC1FilterResult[i]);
        }
    }
    // printMsg("\n");
}

void Adc1_Voltage_Check_Service(void)
{
    if (    (ADC1FilterResult[ADC1_RANK_24V] > VOLTAGE_24V_ADC_VALUE_MAX) ||
            (ADC1FilterResult[ADC1_RANK_24V] < VOLTAGE_24V_ADC_VALUE_MIN)   ) {
        ModuleError.bits.vol24V = 1;
    } else if ( (ADC1FilterResult[ADC1_RANK_24V] < (VOLTAGE_24V_ADC_VALUE_MAX - VOLTAGE_24V_ADC_VALUE_DIF)) &&
                (ADC1FilterResult[ADC1_RANK_24V] > (VOLTAGE_24V_ADC_VALUE_MIN + VOLTAGE_24V_ADC_VALUE_DIF)) ) {
        ModuleError.bits.vol24V = 0;
    }

    // printData("ADC1FilterResult[ADC1_RANK_24V] = %d\n", ADC1FilterResult[ADC1_RANK_24V]);
}

void Adc_Handler(void)
{
    if (SYS_TIM_FLAG_100MS) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1_DATA, ADC1_BUFFER_SIZE);
    }

    if (ADC1OverRun) {
        ADC1OverRun = 0;
        printMsg("ADC1 Overrun\n");
    }
    if (ADC1Ready) {
        ADC1Ready = 0;
        Adc1_Get_Result();
        Adc1_Voltage_Check_Service();
    }
}
