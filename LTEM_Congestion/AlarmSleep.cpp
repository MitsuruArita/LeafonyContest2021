#include "AlarmSleep.h"

AlarmSleep::AlarmSleep(uint8_t min)
{
  sleeping_period = min;
}

void AlarmSleep::alarmSleep(void (*callback)(void*))
{
  setAlarm(sleeping_period);
  deepSleep(callback);
}

void AlarmSleep::wakeupSequence()
{
  pinMode(SENS_PWR_ON, OUTPUT);
  digitalWrite(SENS_PWR_ON, HIGH);
  pinMode(STM_RESET_N, OUTPUT);
  digitalWrite(STM_RESET_N, HIGH);
  pinMode(PERI_PWR_ON, OUTPUT);
  digitalWrite(PERI_PWR_ON, HIGH);
  pinMode(LTE_PWR_ON, OUTPUT);
  digitalWrite(LTE_PWR_ON, HIGH);
  //ESPを起こす
  pinMode(TO_ESP, OUTPUT);
  digitalWrite(TO_ESP, LOW);
  pinMode(FROM_ESP, INPUT_PULLUP);
  pmctrl.enableLeaf(true);
  pmctrl.enableSensor(true);
  delay(100);
}

void AlarmSleep::setAlarm(uint8_t period)
{
  rtc.begin();
  rtc.setTime(0, 0, 0);
  rtc.setDate(0, 0, 0);
  rtc.setAlarmDay(0);
  //ほんとは分
  rtc.setAlarmTime(0, period, 0);
  rtc.enableAlarm(rtc.MATCH_DHHMMSS);
}

void AlarmSleep::deepSleep(void (*callback)(void*))
{
  LowPower.begin();
  LowPower.enableWakeupFrom(&rtc, callback, &atime);
  //サンプルに書いてあった
  pinMode(SENS_PWR_ON, OUTPUT);
  digitalWrite(SENS_PWR_ON, LOW);
  pinMode(STM_RESET_N, OUTPUT);
  digitalWrite(STM_RESET_N, LOW);
  pinMode(PERI_PWR_ON, OUTPUT);
  digitalWrite(PERI_PWR_ON, LOW);
  pinMode(LTE_PWR_ON, OUTPUT);
  digitalWrite(LTE_PWR_ON, LOW);
  pmctrl.enableLeaf(false);
  pmctrl.enableSensor(false);
  LowPower.deepSleep();
}
