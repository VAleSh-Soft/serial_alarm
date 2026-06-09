#pragma once
#include <Arduino.h>
#include <shTaskManager.h>
#include "header_file.h"

// ==== пины =========================================
constexpr uint8_t BUZZER_PIN = 10;     // пин для подключения пищалки
constexpr uint8_t ALARM_RED_PIN = 2;   // пин для подключения красного светодиода - индикатора будильника
constexpr uint8_t ALARM_GREEN_PIN = 3; // пин для подключения зеленого светодиода - индикатора будильника

// ==== EEPROM =======================================
#define ALARM_EEPROM_INDEX 50 // индекс в EEPROM для сохранения настроек будильника; индексы 96..99 заняты настройками часов

// ===================================================
shTaskManager tasks(6);

shHandle return_to_default_mode;  // таймер автовозврата в режим показа времени из любого режима настройки
shHandle display_guard;           // вывод данных будильника на экран
shHandle alarm_guard;             // отслеживание будильника
shHandle alarm_buzzer;            // пищалка будильника
shHandle show_alarm_setting_mode; // режим показа настроек будильника
shHandle set_time_mode;           // режим настройки будильника

// ===================================================

enum saAlarmSettingDataType : uint8_t
{
  ALARM_DATA_NO,
  ALARM_DATA_ON_OFF,
  ALARM_DATA_HOUR_1,
  ALARM_DATA_MINUTE_1,
  ALARM_DATA_HOUR_2,
  ALARM_DATA_MINUTE_2,
  ALARM_DATA_INTERVAL
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

// ==== опрос кнопок =================================
void checkButton();

// ==== задачи =======================================
void returnToDefMode();
void showTimeSetting();
void showAlarmSetting();
void setDisplayData();
void checkAlarm();
void runAlarmBuzzer();

void showTimeData(uint8_t hour, uint8_t minute);

// ==== вывод данных =================================
/**
 * @brief вывод на экран данных по состоянию будильника
 *
 * @param _state состояние будильника - включен/выключен
 */
void showAlarmState(uint8_t _state);

/**
 * @brief вывод на экран текстовой метки настраиваемого типа данных будильника
 *
 * @param mode режим экрана настройки
 */
void showSettingType(saAlarmSettingDataType _type);

// // ==== часы =========================================
// /**
//  * @brief сохранение времени после настройки
//  *
//  * @param hour часы
//  * @param minute минуты
//  */
// void saveTime(uint8_t hour, uint8_t minute);

// ==== разное =======================================
/**
 * @brief изменение данных на одну единицу с контролем выхода за предельное значение
 *
 * @param dt изменяемые данные
 * @param max максимальное значение
 * @param toUp направление изменения данных
 */
void checkData(uint8_t &dt, uint8_t max, bool toUp);

/**
 * @brief изменение данных на заданное значение с контролем выхода за предельные значение
 *
 * @param dt изменяемые данные
 * @param min нижняя граница значения
 * @param max верхняя граница значения
 * @param x величина изменения значения
 * @param toUp направление изменения данных
 */
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
