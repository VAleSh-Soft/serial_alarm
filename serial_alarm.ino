#include <Wire.h>
#include <EEPROM.h>
#include "clockSetting.h"
#include <shSimpleClock.h>
#include "header_file.h"
#include "alarm.h"

// ===================================================

SerialAlarm saAlarm(ALARM_RED_PIN, ALARM_GREEN_PIN, ALARM_EEPROM_INDEX);
shSimpleClock saClock;

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
        // тут добавить сброс кнопки
        saClock.setButtonFlag(btn, CLK_BTN_FLAG_NONE);
        return;
      }
    }
  }

  if (saClock.getDisplayMode() == DISPLAY_MODE_SHOW_TIME)
  {
    // иначе ловим двойной клик кнопки Set для включения режима настройки будильника
    if (saClock.getButtonState(CLK_BTN_SET) == BTN_DBLCLICK)
    {
      saClock.setDisplayMode(DISPLAY_MODE_CUSTOM_1);
      saClock.setButtonFlag(CLK_BTN_SET, CLK_BTN_FLAG_NONE);
    }
    // или клик кнопкой Down для вывода на экран настроек будильника
    if (saClock.getButtonState(CLK_BTN_DOWN) == BTN_DBLCLICK)
    {
      saClock.setDisplayMode(DISPLAY_MODE_CUSTOM_2);
      saClock.setButtonFlag(CLK_BTN_DOWN, CLK_BTN_FLAG_NONE);
    }    
  }
}

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

void showAlarmSetting()
{
  // static uint8_t n = 0;
  // static uint8_t k = 0;

  // if (!tasks.getTaskState(show_alarm_setting_mode))
  // {
  //   tasks.startTask(show_alarm_setting_mode);
  //   tasks.startTask(return_to_default_mode);
  //   n = 0;
  //   k = 0;
  // }

  // uint16_t x = 0;
  // DisplayMode m = DISPLAY_MODE_SET_ALARM_HOUR_1;
  // switch (k)
  // {
  // case 0:
  //   m = DISPLAY_MODE_SET_ALARM_HOUR_1;
  //   x = saAlarm.getAlarmPoint1();
  //   break;
  // case 1:
  //   m = DISPLAY_MODE_SET_ALARM_HOUR_2;
  //   x = saAlarm.getAlarmPoint2();
  //   break;
  // case 2:
  //   m = DISPLAY_MODE_SET_ALARM_INTERVAL;
  //   x = saAlarm.getAlarmInterval();
  //   break;
  // }

  // (n < 8) ? showSettingType(m) : showTimeData(x / 60, x % 60);

  // if (++n > 19)
  // {
  //   n = 0;
  //   if (++k > 2)
  //   {
  //     returnToDefMode();
  //   }
  // }
}

void checkAlarm()
{
  // saAlarm.tick(saClock.getCurTime());
  // if (saAlarm.getAlarmState() == ALARM_YES && !tasks.getTaskState(alarm_buzzer))
  // {
  //   runAlarmBuzzer();
  // }
}

void runAlarmBuzzer()
{
  // static uint8_t n = 0;
  // static uint8_t k = 0;
  // // "мелодия" пищалки: первая строка - частота, вторая строка - длительность
  // static const PROGMEM uint32_t pick[2][8] = {
  //     {2000, 0, 2000, 0, 2000, 0, 2000, 0},
  //     {70, 70, 70, 70, 70, 70, 70, 510}};

  // if (!tasks.getTaskState(alarm_buzzer))
  // {
  //   tasks.startTask(alarm_buzzer);
  //   n = 0;
  //   k = 0;
  // }
  // else if (saAlarm.getAlarmState() != ALARM_YES)
  // { // остановка пищалки, если будильник отключен
  //   tasks.stopTask(alarm_buzzer);
  //   return;
  // }

  // tone(BUZZER_PIN, pgm_read_dword(&pick[0][n]), pgm_read_dword(&pick[1][n]));
  // tasks.setTaskInterval(alarm_buzzer, pgm_read_dword(&pick[1][n]), true);
  // if (++n >= 8)
  // {
  //   n = 0;
  //   if (++k >= ALARM_DURATION)
  //   { // остановка пищалки через заданное число секунд
  //     k = 0;
  //     tasks.stopTask(alarm_buzzer);
  //     tasks.setTaskInterval(alarm_buzzer, 50, false);
  //     saAlarm.setAlarmState(ALARM_ON);
  //   }
  // }
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

// void showSettingType(DisplayMode mode)
// {
// // DISPLAY_MODE_SET_ALARM_HOUR_1   - P1:
// // DISPLAY_MODE_SET_ALARM_HOUR_2   - P2:
// // DISPLAY_MODE_SET_ALARM_INTERVAL - It:
// switch (mode)
// {
// case DISPLAY_MODE_SET_ALARM_HOUR_1:
//   disp.setDispData(0, 0b01110011);
//   disp.setDispData(1, 0b10000110);
//   break;
// case DISPLAY_MODE_SET_ALARM_HOUR_2:
//   disp.setDispData(0, 0b01110011);
//   disp.setDispData(1, 0b11011011);
//   break;
// case DISPLAY_MODE_SET_ALARM_INTERVAL:
//   disp.setDispData(0, 0b00000110);
//   disp.setDispData(1, 0b11111000);
//   break;
// default:
//   break;
// }
// disp.setDispData(2, 0x00);
// disp.setDispData(3, 0x00);
// }

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

// ===================================================
void setup()
{
  saClock.init();
  saAlarm.init(saClock.getCurrentDateTime());
}

void loop()
{
  checkButton();
  saClock.tick();
  saAlarm.tick(saClock.getCurrentDateTime());
}
