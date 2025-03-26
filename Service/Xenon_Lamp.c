#include "Xenon_Lamp.h"
#include "MainRegister.h"
#include "Adc_Service.h"
#include "dac.h"
#include <math.h>

Xenon_Ctrl_Step_t XenonLampCtrlStep = ESTABLISH_CURR;
uint16_t XenonLampCtrlTimer = 0;
int16_t XenonLampCtrlCnt = 0;

float XenonLampVol = 0.f;
float XenonLampCurrSet = 0.f;
float XenonLampCurr = 0.f;

void Set_Xenon_Lamp_Enable(uint8_t newState)
{
    HAL_GPIO_WritePin(LAMP_EN_GPIO_Port, LAMP_EN_Pin, !!newState);
}

void Set_Xenon_Lamp_Curr(float newCurr)
{
    uint16_t newDacValue;

    if (fabs(newCurr) < 1e-6) {
        newDacValue = 0;
    } else {
        newDacValue = newCurr / XENON_LAMP_CONTROLER_CURR_MAX * XENON_LAMP_CONTROLER_DAC_MAX;
    }

    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, newDacValue);
}

void Xenon_Lamp_Service(void)
{
    R_BRIGHTNESS_SET = 27;

    if (SYS_TIM_FLAG_100MS) {
        XenonLampVol = (float)ADC1FilterResult[ADC1_RANK_LMAP_VOLTAGE] * 3.3 / 4095 * 101;
        XenonLampCurr = (float)ADC1FilterResult[ADC1_RANK_LMAP_CURRENT] * 3.3 / 4095 / 0.2;

        if (R_BRIGHTNESS_SET) {
            switch (XenonLampCtrlStep) {
                case ESTABLISH_CURR:
                    Set_Xenon_Lamp_Curr(XENON_LAMP_CONTROLER_CURR_MAX);
                    XenonLampCtrlStep = DRIVE_ENABLE;
                    break;
                case DRIVE_ENABLE:
                    Set_Xenon_Lamp_Enable(ON);
                    XenonLampCtrlTimer = 100;
                    XenonLampCtrlCnt = 0;
                    XenonLampCtrlStep = WAITING_LIGHT_UP;
                    break;
                case WAITING_LIGHT_UP:
                    if (XenonLampCtrlTimer) {
                        XenonLampCtrlTimer--;
                        if ((fabs(XenonLampVol) > 1e-6) && (XenonLampVol < XENON_LAMP_LIFE_IND_VOL) && \
                                (fabs(XenonLampCurr - XENON_LAMP_CONTROLER_CURR_MAX) < (XENON_LAMP_CONTROLER_CURR_MAX * 10 / 100))) {
                            if (++XenonLampCtrlCnt > 2) {
                                XenonLampCtrlCnt = 0;
                                XenonLampCtrlTimer = 0;
                                XenonLampCtrlStep = DIMMING;
                            }
                        } else {
                            XenonLampCtrlCnt = 0;
                        }
                    } else {
                        XenonLampCtrlStep = TURN_ON_FAILED;
                        ModuleError.bits.lightDamage = 1;
                    }
                    break;
                case DIMMING:
                    Set_Xenon_Lamp_Curr((float)R_BRIGHTNESS_SET / R_BRIGHTNESS_SET_MAX * XENON_LAMP_CURR_MAX);
                    if (XenonLampVol > XENON_LAMP_LIFE_IND_VOL) {
                        if (XenonLampCtrlCnt < 0) {
                            XenonLampCtrlCnt = 0;
                        }
                        if (++XenonLampCtrlCnt > 100) {
                            XenonLampCtrlCnt = 100;
                            ModuleError.bits.lightHardwareLife = 1;
                        }
                    } else {
                        if (XenonLampCtrlCnt > 0) {
                            XenonLampCtrlCnt = 0;
                        }
                        if (--XenonLampCtrlCnt < -100) {
                            XenonLampCtrlCnt = -100;
                            ModuleError.bits.lightHardwareLife = 0;
                        }
                    }
                    break;
                case TURN_ON_FAILED:
                    Set_Xenon_Lamp_Enable(OFF);
                    Set_Xenon_Lamp_Curr(0);
                    break;
                default:
                    break;
            }
        } else {
            XenonLampCtrlStep = ESTABLISH_CURR;
            XenonLampCtrlTimer = 0;
            XenonLampCtrlCnt = 0;
            Set_Xenon_Lamp_Enable(OFF);
            Set_Xenon_Lamp_Curr(0);

            ModuleError.bits.lightDamage = 0;
        }

        // printf("SP %d, %d, %.2f, %.2f\n", R_BRIGHTNESS_SET, XenonLampCtrlStep, XenonLampVol, XenonLampCurr);
    }

    if (SYS_TIM_FLAG_500MS) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
}