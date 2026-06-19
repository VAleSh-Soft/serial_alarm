#include <Wire.h>
#include <EEPROM.h>
#include "clockSetting.h"
#include <shSimpleClock.h>
#include "header_file.h"
#include "alarm.h"
#include "custom_display.h"

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

  static bool time_changed = false;

  switch (saClock.getDisplayMode())
  {
  // в режиме показа времени
  case DISPLAY_MODE_SHOW_TIME:
    // кнопка Set
    switch (saClock.getButtonState(CLK_BTN_SET))
    {
    // двойной клик кнопки для включения режима настройки будильника
    case BTN_DBLCLICK:
      saClock.setDisplayMode(DISPLAY_MODE_CUSTOM_2);
      saClock.resetButtonState(CLK_BTN_SET);
      break;
    // клик кнопкой для вывода на экран настроек будильника
    case BTN_ONECLICK:
      if (saAlarm.getAlarmState() != ALARM_OFF)
      {
        saClock.setDisplayMode(DISPLAY_MODE_CUSTOM_1);
        saClock.resetButtonState(CLK_BTN_DOWN);
      }
      break;
    }
    if (time_changed)
    {
      // переинициализировать будильник, если время было изменено
      saAlarm.init(saClock.getCurrentDateTime());
      time_changed = false;
    }
    break;

  // в режиме настройки времени
  case DISPLAY_MODE_SET_HOUR:
  case DISPLAY_MODE_SET_MINUTE:
    // кнопка Set
    if (saClock.getButtonFlag(CLK_BTN_SET) == CLK_BTN_FLAG_EXIT ||
        saClock.getButtonFlag(CLK_BTN_SET) == CLK_BTN_FLAG_NEXT)
    {
      // если время было изменено, взвести флаг для переинициализации будильника
      // что время было изменено, поэтому взводим флаг для переинициализации
      // будильника
      time_changed = true;
    }
    break;

  // режиме вывода настроек будильника
  case DISPLAY_MODE_CUSTOM_1:
    // клик кнопкой Set возвращает в режим показа времени
    if (saClock.getButtonState(CLK_BTN_SET) == BTN_ONECLICK)
    {
      saClock.stopTask(show_alarm_setting_mode);
      saClock.setDisplayMode(DISPLAY_MODE_SHOW_TIME);
    }
    break;

  default:
    break;
  }
}

// ===================================================

void returnToDefaultMode()
{
  switch (saClock.getDisplayMode())
  {
  case DISPLAY_MODE_CUSTOM_2:
    saClock.setButtonFlag(CLK_BTN_SET, CLK_BTN_FLAG_EXIT);
    break;
  default:
    break;
  }
  saClock.stopTask(return_to_def_mode);
}

void setDisplayData()
{
  switch (saClock.getDisplayMode())
  {
  case DISPLAY_MODE_CUSTOM_1:
    if (!saClock.getTaskState(show_alarm_setting_mode))
    {
      showAlarmSetting();
    }
    break;
  case DISPLAY_MODE_CUSTOM_2:
    if (!saClock.getTaskState(set_alarm_mode))
    {
      if (saAlarmDataType == ALARM_DATA_NO)
      {
        saAlarmDataType = ALARM_DATA_ON_OFF;
      }
      showAlarmSettingInterface();
    }
    break;
  default:
    break;
  }
}

void checkAlarm()
{
  saAlarm.tick(saClock.getCurrentDateTime());
  if (saAlarm.getAlarmState() == ALARM_YES && !saClock.getTaskState(alarm_buzzer))
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

  if (!saClock.getTaskState(alarm_buzzer))
  {
    saClock.startTask(alarm_buzzer);
    n = 0;
    k = 0;
  }
  else if (saAlarm.getAlarmState() != ALARM_YES)
  { // остановка пищалки, если будильник отключен
    saClock.stopTask(alarm_buzzer);
    return;
  }

  tone(ALARM_BUZZER_PIN, pgm_read_dword(&pick[0][n]), pgm_read_dword(&pick[1][n]));
  saClock.setTaskInterval(alarm_buzzer, pgm_read_dword(&pick[1][n]), true);
  if (++n >= 8)
  {
    n = 0;
    if (++k >= ALARM_DURATION)
    { // остановка пищалки через заданное число секунд
      k = 0;
      saClock.stopTask(alarm_buzzer);
      saClock.setTaskInterval(alarm_buzzer, 50, false);
      saAlarm.setAlarmState(ALARM_ON);
    }
  }
}

// ===================================================
void setup()
{
  saClock.setAdditionalTaskCount(6);
  saClock.init();
  saAlarm.init(saClock.getCurrentDateTime());

  return_to_def_mode = saClock.addAdditionalTask(AUTO_EXIT_TIMEOUT * 1000ul, returnToDefaultMode, false);
  show_alarm_setting_mode = saClock.addAdditionalTask(100ul, showAlarmSetting, false);
  display_guard = saClock.addAdditionalTask(50ul, setDisplayData);
  alarm_guard = saClock.addAdditionalTask(200ul, checkAlarm);
  alarm_buzzer = saClock.addAdditionalTask(50ul, runAlarmBuzzer, false);
  set_alarm_mode = saClock.addAdditionalTask(100, showAlarmSettingInterface, false);
}

void loop()
{
  saClock.tick();
  checkButton();
}
