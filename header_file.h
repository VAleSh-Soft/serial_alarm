#pragma once
#include <Arduino.h>

// ==== пины =========================================

// ==== пищалка ======================================
#define BUZZER_PIN 7 // пин для подключения пищалки
// ==== светодиод ====================================
#define ALARM_RED_PIN 8   // пин для подключения красного светодиода - индикатора будильника
#define ALARM_GREEN_PIN 9 // пин для подключения зеленого светодиода - индикатора будильника

// ==== EEPROM =======================================
#define ALARM_EEPROM_INDEX 50 // индекс в EEPROM для сохранения настроек будильника


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
void showAlarmSetting();
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
#ifdef USE_SET_BRIGHTNESS_MODE
void showBrightnessSetting();
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
void showTimeData(uint8_t hour, uint8_t minute);

/**
 * @brief вывод на экран данных по состоянию будильника
 *
 * @param _state состояние будильника - включен/выключен
 */
void showAlarmState(uint8_t _state);

// /**
//  * @brief вывод на экран текстовой метки настраиваемого типа данных будильника
//  *
//  * @param mode режим экрана
//  */
// void showSettingType(DisplayMode mode);

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
