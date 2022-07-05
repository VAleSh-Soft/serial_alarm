#pragma once
#include <Arduino.h>
#include <DS3231.h>

// ==== параметры условной компиляции ================
// #define USE_LIGHT_SENSOR // использовать или нет датчик света на пине А3 для регулировки яркости экрана
// #define USE_DS18B20 // использовать для вывода температуры датчик DS18b20

// ===================================================
enum DisplayMode : uint8_t
{
  DISPLAY_MODE_SHOW_TIME,          // основной режим - вывод времени на индикатор
  DISPLAY_MODE_SET_HOUR,           // режим настройки часов
  DISPLAY_MODE_SET_MINUTE,         // режим настройки минут
  DISPLAY_MODE_ALARM_ON_OFF,       // режим настройки будильника - вкл/выкл
  DISPLAY_MODE_SET_ALARM_HOUR_1,   // режим настройки будильника, начало интервала - часы
  DISPLAY_MODE_SET_ALARM_MINUTE_1, // режим настройки будильника, начало интервала - минуты
  DISPLAY_MODE_SET_ALARM_HOUR_2,   // режим настройки будильника, конец интервала - часы
  DISPLAY_MODE_SET_ALARM_MINUTE_2, // режим настройки будильника, конец интервала - минуты
  DISPLAY_MODE_SET_ALARM_INTERVAL, // режим настройки будильника, интервал срабатывания
  DISPLAY_MODE_SHOW_TEMP           // режим вывода температуры
};

#define BTN_SET_PIN 4      // пин для подключения кнопки Set
#define BTN_DOWN_PIN 6     // пин для подключения кнопки Down
#define BTN_UP_PIN 9       // пин для подключения кнопки Up
#define DISPLAY_CLK_PIN 11 // пин для подключения экрана - CLK
#define DISPLAY_DAT_PIN 10 // пин для подключения экрана - DAT

#define BUZZER_PIN 5           // пин для подключения пищалки
#define ALARM_RED_PIN 7        // пин для подключения красного светодиода - индикатора будильника
#define ALARM_GREEN_PIN 12     // пин для подключения зеленого светодиода - индикатора будильника
#define ALARM_EEPROM_INDEX 100 // индекс в EEPROM для сохранения настроек будильника

#ifdef USE_LIGHT_SENSOR
#define LIGHT_SENSOR_PIN A3 // пин для подключения датчика света
#endif

#ifdef USE_DS18B20
#define DS18B20_PIN 8 // пин для подключения датчика DS18b20
#endif

// ==== опрос кнопок =================================
void checkButton();
void checkSetButton();
void checkUpDownButton();

// ==== задачи =======================================
void rtcNow();
void blink();
void restartBlink();
void returnToDefMode();
void showTimeSetting();
void setDisp();
void checkAlarm();
void runAlarmBuzzer();
void showTemp();
#ifdef USE_DS18B20
void checkDS18b20();
#endif
#ifdef USE_LIGHT_SENSOR
void setBrightness();
#endif

// ==== вывод данных =================================
// вывод данных на экран
void setDisplay();

// вывод на экран времени
void showTime(DateTime dt);

// вывод на экран данных в режиме настройки времени или будильника
void showTimeData(byte hour, byte minute);

// вывод на экран данных по состоянию будильника
void showAlarmState(byte _state);

// вывод настраиваемого типа данных будильника
void showSettingType();

// ==== часы =========================================
// сохранение времени после настройки
void saveTime(byte hour, byte minute);

// ==== разное =======================================
// изменение данных по клику кнопки с контролем выхода за предельное значение
void checkData(byte &dt, byte max, bool toUp);
void checkData(byte &dt, byte min, byte max, byte x, bool toUp);
