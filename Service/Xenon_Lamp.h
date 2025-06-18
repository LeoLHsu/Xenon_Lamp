#ifndef _XENON_LAMP_H_
#define _XENON_LAMP_H_
#include "main.h"

// #define XENON_PRINT

#define XENON_LAMP_CURR_MAX                 (10)        // unit A

#define XENON_LAMP_CONTROLER_CURR_MAX       (12)        // unit A
#define XENON_LAMP_CONTROLER_DAC_MAX        (XENON_LAMP_CONTROLER_CURR_MAX * 0.25 / ADC_DAC_VEF_VOL * 4095)
#define XENON_LAMP_LIFE_IND_VOL             (17)        // unit V

#define R_BRIGHTNESS_SET_MIN                (0)
#define R_BRIGHTNESS_SET_MAX                (100)
#define R_FILTER_SET_MIN                    (0)
#define R_FILTER_SET_MAX                    (1)

#define XENON_LAMP_SHOTDOWN_TMEP            (100)       // unit Celsius
#define XENON_LAMP_ERROR_TMEP               (80)        // unit Celsius
#define XENON_LAMP_SHOTDOWN_LIFE_FACTOR     (0.8f)

typedef enum {
    ESTABLISH_CURR = 0,
    DRIVE_ENABLE,
    WAITING_LIGHT_UP,
    DIMMING,
    TURN_ON_FAILED,
    SHOTDOWN,
    XENON_CTRL_STEP_MAX
} Xenon_Ctrl_Step_t;

extern Xenon_Ctrl_Step_t XenonLampCtrlStep;
extern float XenonLampVol;
extern float XenonLampCurrSet;
extern float XenonLampCurr;
extern float XenonLampTemp;
extern uint32_t XenonLampWorkTimer_1s;
extern uint32_t XenonLampWorkTimer_Min;

extern void Xenon_Lamp_Initial(void);
extern void Xenon_Lamp_Service(void);

#endif