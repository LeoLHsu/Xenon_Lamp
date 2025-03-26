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

void Swap(uint16_t* a, uint16_t* b)
{
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

uint16_t Partition(uint16_t arr[], uint16_t low, uint16_t high)
{
    uint16_t pivot = arr[high];
    uint16_t i = (low - 1);

    for (uint16_t j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            Swap(&arr[i], &arr[j]);
        }
    }
    Swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

uint16_t Quick_Select(uint16_t arr[], uint16_t low, uint16_t high, uint16_t num)
{
    if (low == high) {
        return arr[low];
    }

    int pivot_index = Partition(arr, low, high);

    if (num == pivot_index) {
        return arr[num];
    } else if (num < pivot_index) {
        return Quick_Select(arr, low, pivot_index - 1, num);
    } else {
        return Quick_Select(arr, pivot_index + 1, high, num);
    }
}

uint16_t Find_Median(uint16_t array[], uint16_t num)
{
    uint16_t middleIndex1, middleIndex2;
    uint16_t median1, median2;
    uint16_t median;

    if ((num & 0x01) == 0) {
        middleIndex1 = (num >> 1) - 1;
        middleIndex2 = (num >> 1);
        median1 = Quick_Select(array, 0, num - 1, middleIndex1);
        median2 = Quick_Select(array, 0, num - 1, middleIndex2);
        median = ((median1 + median2) >> 1);
    } else {
        middleIndex1 = (num >> 1);
        median1 = Quick_Select(array, 0, num - 1, middleIndex1);
        median = median1;
    }

    return median;
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

    if (    (ADC1FilterResult[ADC1_RANK_5V] > VOLTAGE_5V_ADC_VALUE_MAX) ||
            (ADC1FilterResult[ADC1_RANK_5V] < VOLTAGE_5V_ADC_VALUE_MIN) ) {
        ModuleError.bits.vol5V = 1;
    } else if ( (ADC1FilterResult[ADC1_RANK_5V] < (VOLTAGE_5V_ADC_VALUE_MAX - VOLTAGE_5V_ADC_VALUE_DIF)) &&
                (ADC1FilterResult[ADC1_RANK_5V] > (VOLTAGE_5V_ADC_VALUE_MIN + VOLTAGE_5V_ADC_VALUE_DIF))  ) {
        ModuleError.bits.vol5V = 0;
    }
    // printData("ADC1FilterResult[ADC1_RANK_24V] = %d\n", ADC1FilterResult[ADC1_RANK_24V]);
    // printData("ADC1FilterResult[ADC1_RANK_N12V] = %d\n", ADC1FilterResult[ADC1_RANK_N12V]);
}

void Adc_Handler(void)
{
    if (SYS_TIM_FLAG_10MS) {
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
