/**
 * @file custom_display.h
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 *
 * @brief Экраны для вывода данных и настройки будильника
 *
 * @version 1.0
 * @date 2026-06-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include <Arduino.h>
#include <shSimpleClock.h>
#include "header_file.h"

// ===================================================

void getData(uint8_t &h, uint8_t &m)
{
  switch (saAlarmDataType)
  {
  case ALARM_DATA_HOUR_1:
  case ALARM_DATA_MINUTE_1:
    h = saAlarm.getAlarmPoint1() / 60u;
    m = saAlarm.getAlarmPoint1() % 60u;
    break;
  case ALARM_DATA_HOUR_2:
  case ALARM_DATA_MINUTE_2:
    h = saAlarm.getAlarmPoint2() / 60u;
    m = saAlarm.getAlarmPoint2() % 60u;
    break;
  case ALARM_DATA_ON_OFF:
    h = (uint8_t)saAlarm.getOnOffAlarm();
    break;
  case ALARM_DATA_INTERVAL:
    h = saAlarm.getAlarmInterval();
    break;
  default:
    break;
  }
}

void saveData(uint8_t h, uint8_t m)
{
  switch (saAlarmDataType)
  {
  case ALARM_DATA_HOUR_1:
  case ALARM_DATA_MINUTE_1:
    saAlarm.setAlarmPoint1(h * 60 + m);
    break;
  case ALARM_DATA_HOUR_2:
  case ALARM_DATA_MINUTE_2:
    saAlarm.setAlarmPoint2(h * 60 + m);
    break;
  case ALARM_DATA_ON_OFF:
    saAlarm.setOnOffAlarm((bool)h);
    break;
  case ALARM_DATA_INTERVAL:
    saAlarm.setAlarmInterval(h);
    break;
  default:
    break;
  }

  saAlarm.init(saClock.getCurrentDateTime());
}

void checkSettingData(uint8_t &h, uint8_t &m, bool dir)
{
  switch (saAlarmDataType)
  {
  case ALARM_DATA_HOUR_1:
  case ALARM_DATA_HOUR_2:
    checkData(h, 23, dir);
    break;
  case ALARM_DATA_MINUTE_1:
  case ALARM_DATA_MINUTE_2:
    checkData(m, 59, dir);
    break;
  case ALARM_DATA_ON_OFF:
    checkData(h, 1, true);
    break;
  case ALARM_DATA_INTERVAL:
    checkData(h, MIN_INTERVAL, MAX_INTERVAL, 10, dir);
    break;
  default:
    break;
  }
}

void showTimeSetting()
{
  static bool time_checked = false;
  static uint8_t curHour = 0;
  static uint8_t curMinute = 0;
  static uint8_t n = 10;

  if (!tasks.getTaskState(set_time_mode))
  {
    tasks.startTask(set_time_mode);
    tasks.startTask(return_to_default_mode);
    if ((uint8_t)saAlarmDataType > (uint8_t)ALARM_DATA_ON_OFF)
    {
      n = 0;
    }
    getData(curHour, curMinute);
    time_checked = false;
  }

  if (n < 8 && (saAlarmDataType == ALARM_DATA_HOUR_1 ||
                saAlarmDataType == ALARM_DATA_HOUR_2 ||
                saAlarmDataType == ALARM_DATA_INTERVAL))
  {
    if (!n)
    {
      clkDisplay.clear();
      n++;
      return;
    }
    n++;
    showSettingType(saAlarmDataType);
    return;
  }

  // опрос кнопок =====================
  if ((uint8_t)saClock.getButtonFlag(CLK_BTN_SET) > (uint8_t)CLK_BTN_FLAG_NONE)
  {
    tasks.restartTask(return_to_default_mode);

    if (time_checked)
    {
      saveData(curHour, curMinute);
      time_checked = false;
    }

    if (saClock.getButtonFlag(CLK_BTN_SET) == CLK_BTN_FLAG_NEXT)
    {
      n = 0;
      if (saAlarmDataType == ALARM_DATA_INTERVAL ||
          (saAlarmDataType == ALARM_DATA_ON_OFF && saAlarm.getAlarmState() == ALARM_OFF) ||
          (saAlarmDataType == ALARM_DATA_MINUTE_2 && saAlarm.getAlarmPoint1() == saAlarm.getAlarmPoint2()))
      {
        // выход из режима настройки по клику кнопкой Set в случае, если:
        // 1. мы находимся в режиме настройки интервала (он последний);
        // 2. если будильник выключен;
        // 3. если время начала и окончания сигнализации одинаково (однократное срабатывание);
        saClock.setButtonFlag(CLK_BTN_SET, CLK_BTN_FLAG_EXIT);
      }
      else
      {
        // в остальных случаях переходим в следующий режим настройки
        saAlarmDataType++;
      }
    }
    if (saClock.getButtonFlag(CLK_BTN_SET) == CLK_BTN_FLAG_EXIT)
    {
      tasks.stopTask(return_to_default_mode);
      tasks.stopTask(set_time_mode);
      saClock.setDisplayMode(DISPLAY_MODE_SHOW_TIME);
    }
    saClock.setButtonFlag(CLK_BTN_SET, CLK_BTN_FLAG_NONE);
  }

  if ((saClock.getButtonFlag(CLK_BTN_UP) == CLK_BTN_FLAG_NEXT) ||
      (saClock.getButtonFlag(CLK_BTN_DOWN, true) == CLK_BTN_FLAG_NEXT))
  {
    tasks.restartTask(return_to_default_mode);
    checkSettingData(curHour,
                     curMinute,
                     (saClock.getButtonFlag(CLK_BTN_UP, true) == CLK_BTN_FLAG_NEXT));
    time_checked = true;
  }

  // вывод данных на экран ============
  switch (saAlarmDataType)
  {
  case ALARM_DATA_ON_OFF:
    showAlarmState(curHour);
    break;
  case ALARM_DATA_INTERVAL:
    showTimeData(curHour / 60, curHour % 60);
    break;
  default:
    showTimeData(curHour, curMinute);
    break;
  }
}

void showTimeData(uint8_t hour, uint8_t minute)
{
  uint8_t n0 = clkDisplay.encodeDigit(hour / 10);
  uint8_t n1 = clkDisplay.encodeDigit(hour % 10);
  uint8_t n2 = clkDisplay.encodeDigit(minute / 10);
  uint8_t n3 = clkDisplay.encodeDigit(minute % 10);

  if (saClock.getDisplayMode() == DISPLAY_MODE_CUSTOM_2)
  { // если наступило время блинка и кнопки Up/Down не нажаты, то стереть соответствующие разряды; при нажатых кнопках Up/Down во время изменения данных ничего не мигает
    if (!saClock.getBlink() &&
        !saClock.isButtonClosed(CLK_BTN_UP) &&
        !saClock.isButtonClosed(CLK_BTN_DOWN))
    {
      switch (saAlarmDataType)
      {
      case ALARM_DATA_HOUR_1:
      case ALARM_DATA_HOUR_2:
        n0 = 0;
        n1 = 0;
        break;
      case ALARM_DATA_MINUTE_1:
      case ALARM_DATA_MINUTE_2:
        n2 = 0;
        n3 = 0;
        break;
      case ALARM_DATA_INTERVAL:
        n0 = 0;
        n1 = 0;
        n2 = 0;
        n3 = 0;
        break;
      default:
        break;
      }
    }
  }

  clkDisplay.setDispData(0, n0);
  clkDisplay.setDispData(1, n1 + 0x80);
  clkDisplay.setDispData(2, n2);
  clkDisplay.setDispData(3, n3);
}

void showAlarmSetting()
{
  static uint8_t n = 0;
  static uint8_t k = 0;

  if (!tasks.getTaskState(show_alarm_setting_mode))
  {
    tasks.startTask(show_alarm_setting_mode);
    n = 0;
    k = 0;
  }

  uint16_t x = 0;
  saAlarmSettingDataType m = ALARM_DATA_HOUR_1;
  switch (k)
  {
  case 0:
    m = ALARM_DATA_HOUR_1;
    x = saAlarm.getAlarmPoint1();
    break;
  case 1:
    m = ALARM_DATA_HOUR_2;
    x = saAlarm.getAlarmPoint2();
    break;
  case 2:
    m = ALARM_DATA_INTERVAL;
    x = saAlarm.getAlarmInterval();
    break;
  }

  (n < 8) ? showSettingType(m) : showTimeData(x / 60, x % 60);

  if (++n > 19)
  {
    n = 0;
    if (++k > 2)
    {
      tasks.stopTask(show_alarm_setting_mode);
      saClock.setDisplayMode(DISPLAY_MODE_SHOW_TIME);
    }
  }
}

void showAlarmState(uint8_t _state)
{
  clkDisplay.setDispData(0, 0b01110111); // "A"
  clkDisplay.setDispData(1, 0b10111000); // "L:"
  clkDisplay.setDispData(2, 0x00);
  if (!saClock.getBlink() &&
      !saClock.isButtonClosed(CLK_BTN_UP) &&
      !saClock.isButtonClosed(CLK_BTN_DOWN))
  {
    clkDisplay.setDispData(3, 0x00);
  }
  else
  {
    clkDisplay.setDispData(3, (_state) ? 0b01011100 : 0b00001000);
  }
}

void showSettingType(saAlarmSettingDataType _type)
{
  // ALARM_DATA_HOUR_1   - P1:
  // ALARM_DATA_HOUR_2   - P2:
  // ALARM_DATA_INTERVAL - It:
  switch (_type)
  {
  case ALARM_DATA_HOUR_1:
    clkDisplay.setDispData(0, 0b01110011);
    clkDisplay.setDispData(1, 0b10000110);
    break;
  case ALARM_DATA_HOUR_2:
    clkDisplay.setDispData(0, 0b01110011);
    clkDisplay.setDispData(1, 0b11011011);
    break;
  case ALARM_DATA_INTERVAL:
    clkDisplay.setDispData(0, 0b00000110);
    clkDisplay.setDispData(1, 0b11111000);
    break;
  default:
    break;
  }
  clkDisplay.setDispData(2, 0x00);
  clkDisplay.setDispData(3, 0x00);
}
