#include <Wire.h>
#include <EEPROM.h>
#include <shTaskManager.h>
#include "clockSetting.h"
#include <shSimpleClock.h>
#include "header_file.h"
#include "alarm.h"
#include "custom_display.h"

// ===================================================

SerialAlarm saAlarm(ALARM_RED_PIN, ALARM_GREEN_PIN, ALARM_EEPROM_INDEX);
shSimpleClock saClock;
shTaskManager tasks(4);

shHandle return_to_default_mode;  // таймер автовозврата в режим показа времени из любого режима настройки
shHandle display_guard;           // вывод данных будильника на экран
shHandle alarm_guard;             // отслеживание будильника
shHandle alarm_buzzer;            // пищалка будильника
shHandle show_alarm_setting_mode; // режим показа настроек будильника

// ===================================================
void checkButton()
{
  // если в данный момент сработал будильник, клик любой кнопкой отключает
  // сигнализатор
  if (saAlarm.getAlarmState() == ALARM_YES)
  {
    for (uint8_t i = 0; i < 3; i++)
    {
      clkButtonType btn = (clkButtonType)i;
      if (saClock.getButtonState(btn) == BTN_DOWN ||
          saClock.getButtonState(btn) == BTN_DBLCLICK)
      {
        saAlarm.setAlarmState(ALARM_ON);
        saClock.resetButtonState(btn);
        return;
      }
    }
  }

  switch (saClock.getDisplayMode())
  {
    // в режиме показа времени
  case DISPLAY_MODE_SHOW_TIME:
    // кнопка Set
    switch (saClock.getButtonState(CLK_BTN_SET))
    {
    // двойной клик кнопки для включения режима настройки будильника
    case BTN_DBLCLICK:
      // saClock.setDisplayMode(DISPLAY_MODE_CUSTOM_2);
      saClock.resetButtonState(CLK_BTN_SET);
      break;
    }
    // кнопка Down
    switch (saClock.getButtonState(CLK_BTN_DOWN))
    {
    // клик кнопкой для вывода на экран настроек будильника
    case BTN_ONECLICK:
      saClock.setDisplayMode(DISPLAY_MODE_CUSTOM_1);
      saClock.resetButtonState(CLK_BTN_DOWN);
      break;
    }
    break;
  default:
    break;
  }
}

void returnToDefMode()
{
//   switch (displayMode)
//   {
//   case DISPLAY_MODE_SET_HOUR:
//   case DISPLAY_MODE_SET_MINUTE:
//   case DISPLAY_MODE_SET_ALARM_HOUR_1:
//   case DISPLAY_MODE_SET_ALARM_MINUTE_1:
//   case DISPLAY_MODE_SET_ALARM_HOUR_2:
//   case DISPLAY_MODE_SET_ALARM_MINUTE_2:
//   case DISPLAY_MODE_SET_ALARM_INTERVAL:
//   case DISPLAY_MODE_ALARM_ON_OFF:
// #ifdef USE_SET_BRIGHTNESS_MODE
// #ifdef USE_LIGHT_SENSOR
//   case DISPLAY_MODE_SET_BRIGHTNESS_MIN:
// #endif
//   case DISPLAY_MODE_SET_BRIGHTNESS_MAX:
// #endif
//     btnSet.setButtonFlag(BTN_FLAG_EXIT);
//     break;
//   case DISPLAY_MODE_SHOW_ALARM_SETTING:
//     displayMode = DISPLAY_MODE_SHOW_TIME;
//     tasks.stopTask(show_alarm_setting_mode);
//     break;
// #ifdef USE_TEMP_DATA
//   case DISPLAY_MODE_SHOW_TEMP:
//     displayMode = DISPLAY_MODE_SHOW_TIME;
//     tasks.stopTask(show_temp_mode);
//     break;
// #endif
//   default:
//     break;
//   }
  tasks.stopTask(return_to_default_mode);
}


void checkAlarm()
{
  saAlarm.tick(saClock.getCurrentDateTime());
  if (saAlarm.getAlarmState() == ALARM_YES && !tasks.getTaskState(alarm_buzzer))
  {
    runAlarmBuzzer();
  }
}

void runAlarmBuzzer()
{
  static uint8_t n = 0;
  static uint8_t k = 0;
  // "мелодия" пищалки: первая строка - частота, вторая строка - длительность
  static const PROGMEM uint32_t pick[2][8] = {
      {2000, 0, 2000, 0, 2000, 0, 2000, 0},
      {70, 70, 70, 70, 70, 70, 70, 510}};

  if (!tasks.getTaskState(alarm_buzzer))
  {
    tasks.startTask(alarm_buzzer);
    n = 0;
    k = 0;
  }
  else if (saAlarm.getAlarmState() != ALARM_YES)
  { // остановка пищалки, если будильник отключен
    tasks.stopTask(alarm_buzzer);
    return;
  }

  tone(BUZZER_PIN, pgm_read_dword(&pick[0][n]), pgm_read_dword(&pick[1][n]));
  tasks.setTaskInterval(alarm_buzzer, pgm_read_dword(&pick[1][n]), true);
  if (++n >= 8)
  {
    n = 0;
    if (++k >= ALARM_DURATION)
    { // остановка пищалки через заданное число секунд
      k = 0;
      tasks.stopTask(alarm_buzzer);
      tasks.setTaskInterval(alarm_buzzer, 50, false);
      saAlarm.setAlarmState(ALARM_ON);
    }
  }
}

// ===================================================
void setup()
{
  saClock.init();
  saAlarm.init(saClock.getCurrentDateTime());

  return_to_default_mode = tasks.addTask(AUTO_EXIT_TIMEOUT * 1000ul, returnToDefMode, false);
  show_alarm_setting_mode = tasks.addTask(100, showAlarmSetting, false);
  display_guard = tasks.addTask(50, setDisp);
  alarm_guard = tasks.addTask(200, checkAlarm);
  alarm_buzzer = tasks.addTask(50, runAlarmBuzzer, false);
}

void loop()
{
  saClock.tick();
  checkButton();
  tasks.tick();
}
