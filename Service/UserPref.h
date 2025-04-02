#ifndef __USERPREF_H__
#define __USERPREF_H__
#include "UserPrefDef.h"
#include "string.h"
#include "main.h"

#define UserSettingLampWorkTimerMin     EEData.User.lampWorkTimer_Min
#define UserSettingLampLifeTimeHr       EEData.User.lampLifeTime_Hr
#define UserSetting                     EEData.User

extern EEPromType EEData;

extern void EE_Read_Setting(void);
extern void EE_Setting_Handler(void);

#endif
