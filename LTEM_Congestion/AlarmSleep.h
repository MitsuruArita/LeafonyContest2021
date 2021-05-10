#ifndef _ALARM_H_
#define _ALARM_H_

#include "STM32LowPower.h"
#include "STM32RTC.h"
#include <Lpwa.h>

#define TO_ESP      PD5   //leaf pin 8
#define FROM_ESP    PD6   //leaf pin 10

class AlarmSleep {
  public:
    AlarmSleep(uint8_t min);
    void alarmSleep(void (*callback)(void*));
    void wakeupSequence();
  private:
    LpwaCtrl pmctrl;
    STM32RTC& rtc = STM32RTC::getInstance();
    uint8_t sleeping_period;
    //なぞ
    uint32_t atime = 567;
    void setAlarm(uint8_t period);
    void deepSleep(void (*callback)(void*));
};

#endif
