#ifndef _XENON_LAMP_H_
#define _XENON_LAMP_H_
#include "main.h"



#define XENON_LAMP_CURR_MAX                 (7.5)       // unit A

#define XENON_LAMP_CONTROLER_CURR_MAX       (10)        // unit A
#define XENON_LAMP_CONTROLER_DAC_MAX        (XENON_LAMP_CONTROLER_CURR_MAX * 0.2 / 2 * 3 / ADC_DAC_VEF_VOL * 4095)
#define XENON_LAMP_LIFE_IND_VOL             (20)        // unit V

#define R_BRIGHTNESS_SET_MIN                (0)
#define R_BRIGHTNESS_SET_MAX                (100)

typedef enum {
    ESTABLISH_CURR = 0,
    DRIVE_ENABLE,
    WAITING_LIGHT_UP,
    DIMMING,
    TURN_ON_FAILED
} Xenon_Ctrl_Step_t;

extern uint32_t XenonLampWorkTimer_1s;
extern uint32_t XenonLampWorkTimer_Min;

extern void Xenon_Lamp_Initial(void);
extern void Xenon_Lamp_Service(void);

#endif