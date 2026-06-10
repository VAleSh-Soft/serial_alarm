#include <Wire.h>
#include <EEPROM.h>
#include <shTaskManager.h>
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
    }
    // кнопка Down
    switch (saClock.getButtonState(CLK_BTN_DOWN))
    {
    // клик кнопкой для вывода на экран настроек будильника
    case BTN_ONECLICK:
      if (saAlarm.getAlarmState() != ALARM_OFF)
      {
        saClock.setDisplayMode(DISPLAY_MODE_CUSTOM_1);
        saClock.resetButtonState(CLK_BTN_DOWN);
      }
      break;
    }
    break;

  // в режиме настройки будильника
  case DISPLAY_MODE_CUSTOM_2:
    break;
  default:
    break;
  }
}

// ===================================================

void returnToDefMode()
{
  switch (saClock.getDisplayMode())
  {
  case DISPLAY_MODE_CUSTOM_2:
    saClock.setButtonFlag(CLK_BTN_SET, CLK_BTN_FLAG_EXIT);
    break;
  default:
    break;
  }
  tasks.stopTask(return_to_default_mode);
}

void setDisplayData()
{
  switch (saClock.getDisplayMode())
  {
  case DISPLAY_MODE_CUSTOM_1:
    if (!tasks.getTaskState(show_alarm_setting_mode))
    {
      showAlarmSetting();
    }
    break;
  case DISPLAY_MODE_CUSTOM_2:
    if (!tasks.getTaskState(set_time_mode))
    {
      saAlarmDataType = ALARM_DATA_ON_OFF;
      showTimeSetting();
    }
    break;
  default:
    break;
  }
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
  Serial.begin(115200);
  saClock.init();
  saAlarm.init(saClock.getCurrentDateTime());

  return_to_default_mode = tasks.addTask(AUTO_EXIT_TIMEOUT * 1000ul, returnToDefMode, false);
  show_alarm_setting_mode = tasks.addTask(100ul, showAlarmSetting, false);
  display_guard = tasks.addTask(50ul, setDisplayData);
  alarm_guard = tasks.addTask(200ul, checkAlarm);
  alarm_buzzer = tasks.addTask(50ul, runAlarmBuzzer, false);
  set_time_mode = tasks.addTask(100, showTimeSetting, false);
}

void loop()
{
  saClock.tick();
  checkButton();
  tasks.tick();
}
