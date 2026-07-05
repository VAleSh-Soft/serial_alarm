#pragma once
#include <Arduino.h>
#include "header_file.h"

// ==== пины =========================================
constexpr uint8_t ALARM_BUZZER_PIN = 7; // пин для подключения пищалки
constexpr uint8_t ALARM_RED_PIN = 2;    // пин для подключения красного светодиода - индикатора будильника
constexpr uint8_t ALARM_GREEN_PIN = 3;  // пин для подключения зеленого светодиода - индикатора будильника

// ==== EEPROM =======================================
#define ALARM_EEPROM_INDEX 50 // индекс в EEPROM для сохранения настроек будильника; индексы 96..99 заняты настройками часов

// ===================================================
clkHandle buttons_guard;           // опрос кнопок
clkHandle return_to_def_mode;      // таймер автовозврата в режим показа времени из любого режима настройки
clkHandle display_guard;           // вывод данных будильника на экран
clkHandle alarm_guard;             // отслеживание будильника
clkHandle alarm_buzzer;            // пищалка будильника
clkHandle show_alarm_setting_mode; // режим показа текущих настроек будильника или списка точек срабатывания
clkHandle set_alarm_mode;          // режим настройки будильника

// ===================================================

enum saAlarmSettingDataType : uint8_t
{
  ALARM_DATA_NO,
  ALARM_DATA_ON_OFF,
  ALARM_DATA_HOUR_1,
  ALARM_DATA_MINUTE_1,
  ALARM_DATA_HOUR_2,
  ALARM_DATA_MINUTE_2,
  ALARM_DATA_INTERVAL,
  ALARM_DATA_NEXT_POINT,
  ALARM_DATA_PONT_LIST
};

static saAlarmSettingDataType getNext(const saAlarmSettingDataType current)
{
  switch (current)
  {
  case ALARM_DATA_NO:
  case ALARM_DATA_ON_OFF:
  case ALARM_DATA_HOUR_1:
  case ALARM_DATA_MINUTE_1:
  case ALARM_DATA_HOUR_2:
  case ALARM_DATA_MINUTE_2:
  case ALARM_DATA_INTERVAL:
  case ALARM_DATA_PONT_LIST:
    uint8_t x;
    x = (uint8_t)current;
    return (saAlarmSettingDataType)++x;
  default:;
  }
  return ALARM_DATA_NO;
}

// префиксный оператор (++obj)
saAlarmSettingDataType &operator++(saAlarmSettingDataType &obj)
{
  obj = getNext(obj);
  return obj;
}

// постфиксный оператор (obj++)
saAlarmSettingDataType operator++(saAlarmSettingDataType &obj, const int)
{
  const saAlarmSettingDataType copy = obj;
  obj = getNext(obj);
  return copy;
}

saAlarmSettingDataType saAlarmDataType = ALARM_DATA_NO;

// ===================================================

shSimpleClock saClock;

// ==== задачи =======================================
void checkButton();
void returnToDefaultMode();
void showAlarmSettingInterface();
void showAlarmSetting();
void setDisplayData();
void checkAlarm();
void runAlarmBuzzer();

// ==== вывод данных =================================
void showTimeData(uint8_t hour, uint8_t minute);
void saveData(uint8_t h, uint8_t m);
void showAlarmState(uint8_t _state);
void showSettingType(saAlarmSettingDataType _type);
void checkData(uint8_t &dt, uint8_t max, bool toUp);
void checkData(uint8_t &dt, uint8_t min, uint8_t max, uint8_t x, bool toUp);

// ===================================================

void checkData(uint8_t &dt, uint8_t max, bool toUp)
{
  (toUp) ? dt++ : dt--;
  if (dt > max)
  {
    dt = (toUp) ? 0 : max;
  }
}

void checkData(uint8_t &dt, uint8_t min, uint8_t max, uint8_t x, bool toUp)
{
  (toUp) ? dt += x : dt -= x;
  if (dt < min)
  {
    dt = min;
  }
  else if (dt > max)
  {
    dt = max;
  }
}
