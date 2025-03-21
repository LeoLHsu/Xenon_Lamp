#include "SystemTimeBase.h"

volatile uint8_t SystemTimeBase_1ms_Cnt_1ms;
volatile uint8_t SystemTimeBase_5ms_Cnt_1ms;
volatile uint8_t SystemTimeBase_10ms_Cnt_5ms;
volatile uint8_t SystemTimeBase_100ms_Cnt_10ms;
volatile uint8_t SystemTimeBase_200ms_Cnt_100ms;
volatile uint8_t SystemTimeBase_500ms_Cnt_100ms;
volatile uint8_t SystemTimeBase_1000ms_Cnt_500ms;

uint32_t SystemTimeBase_Second;
volatile uint8_t SystemTimeBase_Flag;

#define SYS_TIM_SET_FLAG_1MS()      (SystemTimeBase_Flag |= SYS_TIM_BIT_1MS)
#define SYS_TIM_CLR_FLAG_1MS()      (SystemTimeBase_Flag &= (uint8_t)~SYS_TIM_BIT_1MS)

#define SYS_TIM_SET_FLAG_5MS()      (SystemTimeBase_Flag |= SYS_TIM_BIT_5MS)
#define SYS_TIM_CLR_FLAG_5MS()      (SystemTimeBase_Flag &= (uint8_t)~SYS_TIM_BIT_5MS)

#define SYS_TIM_SET_FLAG_10MS()     (SystemTimeBase_Flag |= SYS_TIM_BIT_10MS)
#define SYS_TIM_CLR_FLAG_10MS()     (SystemTimeBase_Flag &= (uint8_t)~SYS_TIM_BIT_10MS)

#define SYS_TIM_SET_FLAG_100MS()    (SystemTimeBase_Flag |= SYS_TIM_BIT_100MS)
#define SYS_TIM_CLR_FLAG_100MS()    (SystemTimeBase_Flag &= (uint8_t)~SYS_TIM_BIT_100MS)

#define SYS_TIM_SET_FLAG_200MS()    (SystemTimeBase_Flag |= SYS_TIM_BIT_200MS)
#define SYS_TIM_CLR_FLAG_200MS()    (SystemTimeBase_Flag &= (uint8_t)~SYS_TIM_BIT_200MS)

#define SYS_TIM_SET_FLAG_500MS()    (SystemTimeBase_Flag |= SYS_TIM_BIT_500MS)
#define SYS_TIM_CLR_FLAG_500MS()    (SystemTimeBase_Flag &= (uint8_t)~SYS_TIM_BIT_500MS)

#define SYS_TIM_SET_FLAG_1000MS()   (SystemTimeBase_Flag |= SYS_TIM_BIT_1000MS)
#define SYS_TIM_CLR_FLAG_1000MS()   (SystemTimeBase_Flag &= (uint8_t)~SYS_TIM_BIT_1000MS)

void SystemTimeBase_Initial(void)
{
    SystemTimeBase_Second = 0;
    SystemTimeBase_1ms_Cnt_1ms = 0;
    SystemTimeBase_5ms_Cnt_1ms = 0;
    SystemTimeBase_10ms_Cnt_5ms = 0;
    SystemTimeBase_100ms_Cnt_10ms = 0;
    SystemTimeBase_200ms_Cnt_100ms = 0;
    SystemTimeBase_500ms_Cnt_100ms = 0;
    SystemTimeBase_1000ms_Cnt_500ms = 0;
    SystemTimeBase_Flag = 0;
}

void SystemTimeBase_Service_ISR(void)
{
    SystemTimeBase_1ms_Cnt_1ms++;
}

void SystemTimeBase_DelayXms(uint16_t Counter)
{
    HAL_Delay(Counter);
}

void SystemTimeBase_DelayXus(uint16_t Counter)
{
    uint32_t tickStart, tickCur, tickCnt;
    uint32_t tickMax = SysTick->LOAD;
    uint32_t delayValue = (SysTick->LOAD / 1000) * Counter;

    if (delayValue <= tickMax) {
        tickStart = SysTick->VAL;
        while (1) {
            tickCur = SysTick->VAL;
            tickCnt = (tickStart >= tickCur) ? (tickStart - tickCur) : (tickStart + (tickMax - tickCur));
            if (tickCnt >= delayValue) {
                break;
            }
        }
    }
}

void SystemTimeBase_Set_Handler(void)
{
    if (SystemTimeBase_1ms_Cnt_1ms >= 1) {
        SystemTimeBase_1ms_Cnt_1ms = 0;
        SYS_TIM_SET_FLAG_1MS();
        if (++SystemTimeBase_5ms_Cnt_1ms >= 5) {
            SystemTimeBase_5ms_Cnt_1ms = 0;
            SYS_TIM_SET_FLAG_5MS();
            if (++SystemTimeBase_10ms_Cnt_5ms >= 2) {
                SystemTimeBase_10ms_Cnt_5ms = 0;
                SYS_TIM_SET_FLAG_10MS();
                if (++SystemTimeBase_100ms_Cnt_10ms >= 10) {
                    SystemTimeBase_100ms_Cnt_10ms = 0;
                    SYS_TIM_SET_FLAG_100MS();
                    if (++SystemTimeBase_200ms_Cnt_100ms >= 2) {
                        SystemTimeBase_200ms_Cnt_100ms = 0;
                        SYS_TIM_SET_FLAG_200MS();
                    }
                    if (++SystemTimeBase_500ms_Cnt_100ms >= 5) {
                        SystemTimeBase_500ms_Cnt_100ms = 0;
                        SYS_TIM_SET_FLAG_500MS();
                        if (++SystemTimeBase_1000ms_Cnt_500ms >= 2) {
                            SystemTimeBase_1000ms_Cnt_500ms = 0;
                            SYS_TIM_SET_FLAG_1000MS();
                        }
                    }
                }
            }
        }
    }
}

void SystemTimeBase_Clr_Handler(void)
{
    if (SYS_TIM_FLAG_1000MS) {
        SystemTimeBase_Second++;
    }
    SYS_TIM_CLR_FLAG_1MS();
    SYS_TIM_CLR_FLAG_5MS();
    SYS_TIM_CLR_FLAG_10MS();
    SYS_TIM_CLR_FLAG_100MS();
    SYS_TIM_CLR_FLAG_200MS();
    SYS_TIM_CLR_FLAG_500MS();
    SYS_TIM_CLR_FLAG_1000MS();
}
