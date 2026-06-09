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
  // switch (displayMode)
  // {
  // case DISPLAY_MODE_SET_ALARM_HOUR_1:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  //   h = saAlarm.getAlarmPoint1() / 60;
  //   m = saAlarm.getAlarmPoint1() % 60;
  //   break;
  // case DISPLAY_MODE_SET_ALARM_HOUR_2:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  //   h = saAlarm.getAlarmPoint2() / 60;
  //   m = saAlarm.getAlarmPoint2() % 60;
  //   break;
  // case DISPLAY_MODE_ALARM_ON_OFF:
  //   h = (uint8_t)saAlarm.getOnOffAlarm();
  //   break;
  // case DISPLAY_MODE_SET_ALARM_INTERVAL:
  //   h = saAlarm.getAlarmInterval();
  //   break;
  // default:
  //   break;
  // }
}

void saveData(uint8_t h, uint8_t m)
{
  // switch (displayMode)
  // {
  // case DISPLAY_MODE_SET_HOUR:
  // case DISPLAY_MODE_SET_MINUTE:
  //   saveTime(h, m);
  //   break;
  // case DISPLAY_MODE_SET_ALARM_HOUR_1:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  //   saAlarm.setAlarmPoint1(h * 60 + m);
  //   break;
  // case DISPLAY_MODE_SET_ALARM_HOUR_2:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  //   saAlarm.setAlarmPoint2(h * 60 + m);
  //   break;
  // case DISPLAY_MODE_ALARM_ON_OFF:
  //   saAlarm.setOnOffAlarm((bool)h);
  //   break;
  // case DISPLAY_MODE_SET_ALARM_INTERVAL:
  //   saAlarm.setAlarmInterval(h);
  //   break;
  // default:
  //   break;
  // }
  // switch (displayMode)
  // {
  // case DISPLAY_MODE_ALARM_ON_OFF:
  // case DISPLAY_MODE_SET_ALARM_HOUR_1:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  // case DISPLAY_MODE_SET_ALARM_HOUR_2:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  // case DISPLAY_MODE_SET_ALARM_INTERVAL:
  //   saAlarm.init(saClock.getCurTime());
  //   break;
  // default:
  //   break;
  // }
}

void checkSettingData(uint8_t &h, uint8_t &m, bool dir)
{
  // switch (displayMode)
  // {
  // case DISPLAY_MODE_SET_HOUR:
  // case DISPLAY_MODE_SET_ALARM_HOUR_1:
  // case DISPLAY_MODE_SET_ALARM_HOUR_2:
  //   checkData(h, 23, dir);
  //   break;
  // case DISPLAY_MODE_SET_MINUTE:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  // case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  //   checkData(m, 59, dir);
  //   break;
  // case DISPLAY_MODE_ALARM_ON_OFF:
  //   checkData(h, 1, true);
  //   break;
  // case DISPLAY_MODE_SET_ALARM_INTERVAL:
  //   checkData(h, MIN_INTERVAL, MAX_INTERVAL, 10, dir);
  //   break;
  // default:
  //   break;
  // }
}

void showTimeSetting()
{
  // static bool time_checked = false;
  // static uint8_t curHour = 0;
  // static uint8_t curMinute = 0;
  // static uint8_t n = 10;

  // if (!tasks.getTaskState(set_time_mode))
  // {
  //   tasks.startTask(set_time_mode);
  //   tasks.startTask(return_to_default_mode);
  //   restartBlink();
  //   if (displayMode > DISPLAY_MODE_ALARM_ON_OFF)
  //   {
  //     n = 0;
  //   }
  //   getData(curHour, curMinute);
  //   time_checked = false;
  // }

  // if (n < 8 && (displayMode == DISPLAY_MODE_SET_ALARM_HOUR_1 ||
  //               displayMode == DISPLAY_MODE_SET_ALARM_HOUR_2 ||
  //               displayMode == DISPLAY_MODE_SET_ALARM_INTERVAL))
  // {
  //   n++;
  //   showSettingType(displayMode);
  //   return;
  // }

  // if (!time_checked && (displayMode == DISPLAY_MODE_SET_HOUR ||
  //                       displayMode == DISPLAY_MODE_SET_MINUTE))
  // {
  //   curHour = saClock.getCurTime().hour();
  //   curMinute = saClock.getCurTime().minute();
  // }

  // // опрос кнопок =====================
  // if (btnSet.getButtonFlag() > BTN_FLAG_NONE)
  // {
  //   if (time_checked)
  //   {
  //     saveData(curHour, curMinute);
  //     time_checked = false;
  //   }
  //   setDisplayMode(curHour);
  //   btnSet.setButtonFlag(BTN_FLAG_NONE);
  // }

  // if ((btnUp.getButtonFlag() == BTN_FLAG_NEXT) ||
  //     (btnDown.getButtonFlag(true) == BTN_FLAG_NEXT))
  // {
  //   checkSettingData(curHour, curMinute, (btnUp.getButtonFlag(true) == BTN_FLAG_NEXT));
  //   time_checked = true;
  // }

  // // вывод данных на экран ============
  // switch (displayMode)
  // {
  // case DISPLAY_MODE_ALARM_ON_OFF:
  //   showAlarmState(curHour);
  //   break;
  // case DISPLAY_MODE_SET_ALARM_INTERVAL:
  //   showTimeData(curHour / 60, curHour % 60);
  //   break;
  // default:
  //   showTimeData(curHour, curMinute);
  //   break;
  // }
}

void showTimeData(uint8_t h, uint8_t m)
{
  clkDisplay.setDispData(0, clkDisplay.encodeDigit(h / 10));
  clkDisplay.setDispData(1, clkDisplay.encodeDigit(h % 10) + 0x80);
  clkDisplay.setDispData(2, clkDisplay.encodeDigit(m / 10));
  clkDisplay.setDispData(3, clkDisplay.encodeDigit(m % 10));
}

void showAlarmSetting()
{
  static uint8_t n = 0;
  static uint8_t k = 0;

  if (!tasks.getTaskState(show_alarm_setting_mode))
  {
    tasks.startTask(show_alarm_setting_mode);
    tasks.startTask(return_to_default_mode);
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
      returnToDefMode();
    }
  }
}

void showAlarmState(uint8_t _state)
{
  // disp.setDispData(0, 0b01110111); // "A"
  // disp.setDispData(1, 0b10111000); // "L:"
  // disp.setDispData(2, 0x00);
  // if (!blink_flag && !btnUp.isButtonClosed() && !btnDown.isButtonClosed())
  // {
  //   disp.setDispData(3, 0x00);
  // }
  // else
  // {
  //   disp.setDispData(3, (_state) ? 0b01011100 : 0b00001000);
  // }
}

void showSettingType(saAlarmSettingDataType _type)
{
  // ALARM_DATA_ON_OFF   - Al
  // ALARM_DATA_HOUR_1   - P1:
  // ALARM_DATA_HOUR_2   - P2:
  // ALARM_DATA_INTERVAL - It:
  switch (_type)
  {
  case ALARM_DATA_ON_OFF:
    clkDisplay.setDispData(0, 0b01110111);
    clkDisplay.setDispData(1, 0b00111000);
    break;
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
