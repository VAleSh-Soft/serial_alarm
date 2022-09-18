#pragma once
#include <Arduino.h>
#include <DS3231.h> // https://github.com/NorthernWidget/DS3231

// ==== параметры условной компиляции ================
// #define USE_LIGHT_SENSOR // использовать или нет датчик света на пине А3 для регулировки яркости экрана
// #define USE_TEMP_DATA // использовать или нет вывод температуры по клику кнопкой Up
#ifdef USE_TEMP_DATA
// #define USE_DS18B20 // использовать для вывода температуры датчик DS18b20
#endif

// ==== пины =========================================

// ==== кнопки =======================================
#define BTN_SET_PIN 4  // пин для подключения кнопки Set
#define BTN_DOWN_PIN 6 // пин для подключения кнопки Down
#define BTN_UP_PIN 9   // пин для подключения кнопки Up
// ==== экран ========================================
#define DISPLAY_CLK_PIN 11 // пин для подключения экрана - CLK
#define DISPLAY_DAT_PIN 10 // пин для подключения экрана - DAT
// ==== DS3231 =======================================
#define DS3231_SDA_PIN A4 // пин для подключения вывода SDA модуля DS3231 (не менять!!!)
#define DS3231_SCL_PIN A5 // пин для подключения вывода SCL модуля DS3231 (не менять!!!)
// ==== пищалка ======================================
#define BUZZER_PIN 5 // пин для подключения пищалки
// ==== светодиод ====================================
#define ALARM_RED_PIN 7    // пин для подключения красного светодиода - индикатора будильника
#define ALARM_GREEN_PIN 12 // пин для подключения зеленого светодиода - индикатора будильника
// ==== датчики ======================================
#ifdef USE_LIGHT_SENSOR
#define LIGHT_SENSOR_PIN A3 // пин для подключения датчика света
#endif
#ifdef USE_DS18B20
#define DS18B20_PIN 8 // пин для подключения датчика DS18b20
#endif

// ==== EEPROM =======================================
#define ALARM_EEPROM_INDEX 100 // индекс в EEPROM для сохранения настроек будильника

// ==== режимы экрана ================================
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
  DISPLAY_MODE_SET_ALARM_INTERVAL  // режим настройки будильника, интервал срабатывания
#ifdef USE_TEMP_DATA
  ,
  DISPLAY_MODE_SHOW_TEMP // режим вывода температуры
#endif
};

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
#ifdef USE_TEMP_DATA
void showTemp();
#ifdef USE_DS18B20
void checkDS18b20();
#endif
#endif
#ifdef USE_LIGHT_SENSOR
void setBrightness();
#endif

// ==== вывод данных =================================
/**
 * @brief вывод данных на экран
 * 
 */
void setDisplay();

/**
 * @brief вывод на экран данных в режиме настройки времени или будильника
 * 
 * @param hour часы
 * @param minute минуты
 */
void showTimeData(byte hour, byte minute);

/**
 * @brief вывод на экран данных по состоянию будильника
 * 
 * @param _state состояние будильника - включен/выключен
 */
void showAlarmState(byte _state);

/**
 * @brief вывод на экран текстовой метки настраиваемого типа данных будильника
 * 
 */
void showSettingType();

// ==== часы =========================================
/**
 * @brief сохранение времени после настройки
 * 
 * @param hour часы
 * @param minute минуты
 */
void saveTime(byte hour, byte minute);

// ==== разное =======================================
/**
 * @brief изменение данных на одну единицу с контролем выхода за предельное значение
 * 
 * @param dt изменяемые данные
 * @param max максимальное значение
 * @param toUp направление изменения данных
 */
void checkData(byte &dt, byte max, bool toUp);

/**
 * @brief изменение данных на заданное значение с контролем выхода за предельные значение
 * 
 * @param dt изменяемые данные
 * @param min нижняя граница значения
 * @param max верхняя граница значения
 * @param x величина изменения значения
 * @param toUp направление изменения данных
 */
void checkData(byte &dt, byte min, byte max, byte x, bool toUp);
