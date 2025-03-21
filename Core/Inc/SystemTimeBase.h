#ifndef __SYSTEM_TIME_BASE_H__
#define __SYSTEM_TIME_BASE_H__
#include "main.h"

#define SystemTimeBase_Service_ISR    Timer_CallBack_ISR

extern uint32_t SystemTimeBase_Second;
extern volatile uint8_t SystemTimeBase_Flag;

#define SYS_TIM_BIT_1MS             BIT(0)
#define SYS_TIM_FLAG_1MS            (SystemTimeBase_Flag & SYS_TIM_BIT_1MS)

#define SYS_TIM_BIT_5MS             BIT(1)
#define SYS_TIM_FLAG_5MS            (SystemTimeBase_Flag & SYS_TIM_BIT_5MS)

#define SYS_TIM_BIT_10MS            BIT(2)
#define SYS_TIM_FLAG_10MS           (SystemTimeBase_Flag & SYS_TIM_BIT_10MS)

#define SYS_TIM_BIT_100MS           BIT(3)
#define SYS_TIM_FLAG_100MS          (SystemTimeBase_Flag & SYS_TIM_BIT_100MS)

#define SYS_TIM_BIT_200MS           BIT(4)
#define SYS_TIM_FLAG_200MS          (SystemTimeBase_Flag & SYS_TIM_BIT_200MS)

#define SYS_TIM_BIT_500MS           BIT(5)
#define SYS_TIM_FLAG_500MS          (SystemTimeBase_Flag & SYS_TIM_BIT_500MS)

#define SYS_TIM_BIT_1000MS          BIT(6)
#define SYS_TIM_FLAG_1000MS         (SystemTimeBase_Flag & SYS_TIM_BIT_1000MS)

extern void SystemTimeBase_Initial(void);
extern void SystemTimeBase_Set_Handler(void);
extern void SystemTimeBase_Clr_Handler(void);
extern void SystemTimeBase_Service_ISR(void);
extern void SystemTimeBase_DelayXms(uint16_t Counter);
extern void SystemTimeBase_DelayXus(uint16_t Counter);

#endif
