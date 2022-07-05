#include <Wire.h>
#include <DS3231.h>        // https://github.com/NorthernWidget/DS3231
#include <shButton.h>      // https://github.com/VAleSh-Soft/shButton
#include <shTaskManager.h> // https://github.com/VAleSh-Soft/shTaskManager
#include "header_file.h"
#include "display_TM1637.h"
#include "alarm.h"
#ifdef USE_DS18B20
#include <OneWire.h> // https://github.com/PaulStoffregen/OneWire
#endif

// ==== настройки ====================================
#define ALARM_DURATION 60        // продолжительность сигнала будильника, секунд
#define MIN_DISPLAY_BRIGHTNESS 1 // минимальная яркость дисплея, 1-7
#define MAX_DISPLAY_BRIGHTNESS 7 // максимальная яркость дисплея, 1-7
#define LIGHT_THRESHOLD 300      // порог переключения для датчика света
#define AUTO_EXIT_TIMEOUT 6      // время автоматического возврата в режим показа текущего времени из любых других режимов при отсутствии активности пользователя, секунд
// ===================================================

DisplayTM1637 disp(DISPLAY_CLK_PIN, DISPLAY_DAT_PIN);
DS3231 clock; // SDA - A4, SCL - A5
RTClib RTC;
SerialAlarm alarm(ALARM_RED_PIN, ALARM_GREEN_PIN, ALARM_EEPROM_INDEX);
#ifdef USE_DS18B20
OneWire ds(DS18B20_PIN); // вход датчика DS18b20
byte addr[8];            // адрес датчика температуры
int temperature;
#endif

DateTime curTime;

shTaskManager tasks; // создаем список задач, количество задач укажем в setup()

shHandle rtc_guard;              // опрос микросхемы RTC по таймеру, чтобы не дергать ее откуда попало
shHandle blink_timer;            // блинк
shHandle return_to_default_mode; // таймер автовозврата в режим показа времени из любого режима настройки
shHandle set_time_mode;          // режим настройки времени
shHandle display_guard;          // вывод данных на экран
shHandle alarm_guard;            // отслеживание будильника
shHandle alarm_buzzer;           // пищалка будильника
shHandle show_temp_mode;         // режим показа температуры
#ifdef USE_DS18B20
shHandle ds18b20_guard; // опрос датчика DS18b20
#endif
#ifdef USE_LIGHT_SENSOR
shHandle light_sensor_guard; // отслеживание показаний датчика света
#endif

DisplayMode displayMode = DISPLAY_MODE_SHOW_TIME;
bool blink_flag = false; // флаг блинка, используется всем, что должно мигать

// ==== класс кнопок с предварительной настройкой ====
enum ButtonFlag : uint8_t
{
  BTN_FLAG_NONE, // флаг кнопки - ничего не делать
  BTN_FLAG_NEXT, // флаг кнопки - изменить значение
  BTN_FLAG_EXIT  // флаг кнопки - возврат в режим показа текущего времени
};

class clcButton : public shButton
{
private:
  ButtonFlag _flag = BTN_FLAG_NONE;

public:
  clcButton(byte button_pin) : shButton(button_pin)
  {
    shButton::setTimeoutOfLongClick(1000);
    shButton::setLongClickMode(LCM_ONLYONCE);
    shButton::setVirtualClickOn(true);
    shButton::setTimeoutOfDebounce(50);
  }

  ButtonFlag getBtnFlag()
  {
    return (_flag);
  }

  void setBtnFlag(ButtonFlag flag)
  {
    _flag = flag;
  }

  byte getButtonState()
  {
    byte _state = shButton::getButtonState();
    switch (_state)
    {
    case BTN_DOWN:
    case BTN_DBLCLICK:
    case BTN_LONGCLICK:
      // в любом режиме, кроме стандартного, каждый клик любой кнопки перезапускает таймер автовыхода в стандартный режим
      if (tasks.getTaskState(return_to_default_mode))
      {
        tasks.restartTask(return_to_default_mode);
      }
      // если сработал будильник, отключить его
      if (alarm.getAlarmState() == ALARM_YES)
      {
        alarm.setAlarmState(ALARM_ON);
        shButton::resetButtonState();
      }
      break;
    }
    return (_state);
  }
};
// ===================================================

clcButton btnSet(BTN_SET_PIN);   // кнопка Set - смена режима работы часов
clcButton btnUp(BTN_UP_PIN);     // кнопка Up - изменение часов/минут в режиме настройки
clcButton btnDown(BTN_DOWN_PIN); // кнопка Down - изменение часов/минут в режиме настройки

// ===================================================
void checkButton()
{
  checkSetButton();
  checkUpDownButton();
}

void checkSetButton()
{
  switch (btnSet.getButtonState())
  {
  case BTN_ONECLICK:
    switch (displayMode)
    {
    case DISPLAY_MODE_SET_HOUR:
    case DISPLAY_MODE_SET_MINUTE:
    case DISPLAY_MODE_SET_ALARM_HOUR_1:
    case DISPLAY_MODE_SET_ALARM_MINUTE_1:
    case DISPLAY_MODE_SET_ALARM_HOUR_2:
    case DISPLAY_MODE_SET_ALARM_MINUTE_2:
    case DISPLAY_MODE_SET_ALARM_INTERVAL:
    case DISPLAY_MODE_ALARM_ON_OFF:
      btnSet.setBtnFlag(BTN_FLAG_NEXT);
      break;
    default:
      break;
    }
    break;
  case BTN_DBLCLICK:
    switch (displayMode)
    {
    case DISPLAY_MODE_SHOW_TIME:
      displayMode = DISPLAY_MODE_ALARM_ON_OFF;
      break;
    default:
      break;
    }
    break;
  case BTN_LONGCLICK:
    switch (displayMode)
    {
    case DISPLAY_MODE_SHOW_TIME:
      displayMode = DISPLAY_MODE_SET_HOUR;
      break;
    case DISPLAY_MODE_SET_HOUR:
    case DISPLAY_MODE_SET_MINUTE:
    case DISPLAY_MODE_SET_ALARM_HOUR_1:
    case DISPLAY_MODE_SET_ALARM_MINUTE_1:
    case DISPLAY_MODE_SET_ALARM_HOUR_2:
    case DISPLAY_MODE_SET_ALARM_MINUTE_2:
    case DISPLAY_MODE_SET_ALARM_INTERVAL:
    case DISPLAY_MODE_ALARM_ON_OFF:
      btnSet.setBtnFlag(BTN_FLAG_EXIT);
      break;
    default:
      break;
    }
    break;
  }
}

void checkUDbtn(clcButton &btn)
{
  switch (btn.getButtonState())
  {
  case BTN_DOWN:
  case BTN_DBLCLICK:
    btn.setBtnFlag(BTN_FLAG_NEXT);
    break;
  case BTN_LONGCLICK:
    if (displayMode != DISPLAY_MODE_ALARM_ON_OFF)
    {
      btn.setBtnFlag(BTN_FLAG_NEXT);
    }
    break;
  }
}

void checkUpDownButton()
{
  switch (displayMode)
  {
  case DISPLAY_MODE_SHOW_TIME:
    if (btnUp.getButtonState() == BTN_ONECLICK)
    {
      displayMode = DISPLAY_MODE_SHOW_TEMP;
    }
    // опрашивается исключительно чтобы срабатывала на отключение сигнала будильника
    btnDown.getButtonState();
    break;
  case DISPLAY_MODE_SET_HOUR:
  case DISPLAY_MODE_SET_MINUTE:
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
  case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
  case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
  case DISPLAY_MODE_ALARM_ON_OFF:
    checkUDbtn(btnUp);
    checkUDbtn(btnDown);
    break;
  case DISPLAY_MODE_SHOW_TEMP:
    if (btnUp.getButtonState() == BTN_ONECLICK)
    {
      returnToDefMode();
    }
    break;
  default:
    break;
  }
}

// ===================================================
void rtcNow()
{
  curTime = RTC.now();
}

void blink()
{
  if (!tasks.getTaskState(blink_timer))
  {
    tasks.startTask(blink_timer);
    blink_flag = false;
  }
  else
  {
    blink_flag = !blink_flag;
  }
}

void restartBlink()
{
  tasks.stopTask(blink_timer);
  blink();
}

void returnToDefMode()
{
  switch (displayMode)
  {
  case DISPLAY_MODE_SET_HOUR:
  case DISPLAY_MODE_SET_MINUTE:
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
  case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
  case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
  case DISPLAY_MODE_ALARM_ON_OFF:
    btnSet.setBtnFlag(BTN_FLAG_EXIT);
    break;
  case DISPLAY_MODE_SHOW_TEMP:
    displayMode = DISPLAY_MODE_SHOW_TIME;
    tasks.stopTask(show_temp_mode);
    break;
  default:
    break;
  }
  tasks.stopTask(return_to_default_mode);
}

void stopSettintg()
{
  tasks.stopTask(set_time_mode);
  tasks.stopTask(return_to_default_mode);
}

void getData(byte &h, byte &m)
{
  switch (displayMode)
  {
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
  case DISPLAY_MODE_SET_ALARM_MINUTE_1:
    h = alarm.getAlarmPoint1() / 60;
    m = alarm.getAlarmPoint1() % 60;
    break;
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
  case DISPLAY_MODE_SET_ALARM_MINUTE_2:
    h = alarm.getAlarmPoint2() / 60;
    m = alarm.getAlarmPoint2() % 60;
    break;
  case DISPLAY_MODE_ALARM_ON_OFF:
    h = (byte)alarm.getOnOffAlarm();
    break;
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
    h = alarm.getAlarmInterval();
    break;
  default:
    break;
  }
}

void saveData(byte h, byte m)
{
  switch (displayMode)
  {
  case DISPLAY_MODE_SET_HOUR:
  case DISPLAY_MODE_SET_MINUTE:
    saveTime(h, m);
    break;
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
  case DISPLAY_MODE_SET_ALARM_MINUTE_1:
    alarm.setAlarmPoint1(h * 60 + m);
    break;
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
  case DISPLAY_MODE_SET_ALARM_MINUTE_2:
    alarm.setAlarmPoint2(h * 60 + m);
    break;
  case DISPLAY_MODE_ALARM_ON_OFF:
    alarm.setOnOffAlarm((bool)h);
    break;
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
    alarm.setAlarmInterval(h);
    break;
  default:
    break;
  }
  switch (displayMode)
  {
  case DISPLAY_MODE_ALARM_ON_OFF:
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
  case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
  case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
    alarm.init(curTime);
    break;
  default:
    break;
  }
}

void setDisplayMode(byte x)
{
  if (btnSet.getBtnFlag() == BTN_FLAG_NEXT)
  {
    switch (displayMode)
    {
    case DISPLAY_MODE_SET_HOUR:
    case DISPLAY_MODE_SET_ALARM_HOUR_1:
    case DISPLAY_MODE_SET_ALARM_MINUTE_1:
    case DISPLAY_MODE_SET_ALARM_HOUR_2:
      displayMode = DisplayMode(byte(displayMode + 1));
      stopSettintg();
      break;
    case DISPLAY_MODE_SET_ALARM_MINUTE_2:
      displayMode = (alarm.getAlarmPoint1() == alarm.getAlarmPoint2())
                        ? DISPLAY_MODE_SHOW_TIME
                        : DISPLAY_MODE_SET_ALARM_INTERVAL;
      stopSettintg();
      break;
    case DISPLAY_MODE_ALARM_ON_OFF:
      displayMode = (x) ? DISPLAY_MODE_SET_ALARM_HOUR_1 : DISPLAY_MODE_SHOW_TIME;
      stopSettintg();
      break;
    default:
      displayMode = DISPLAY_MODE_SHOW_TIME;
      stopSettintg();
      break;
    }
  }
  else
  {
    displayMode = DISPLAY_MODE_SHOW_TIME;
    stopSettintg();
  }
}

void checkSettingData(byte &h, byte &m, bool dir)
{
  switch (displayMode)
  {
  case DISPLAY_MODE_SET_HOUR:
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
    checkData(h, 23, dir);
    break;
  case DISPLAY_MODE_SET_MINUTE:
  case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  case DISPLAY_MODE_SET_ALARM_MINUTE_2:
    checkData(m, 59, dir);
    break;
  case DISPLAY_MODE_ALARM_ON_OFF:
    checkData(h, 1, true);
    break;
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
    checkData(h, 10, 180, 10, dir);
    break;
  default:
    break;
  }
}

void showTimeSetting()
{
  static bool time_checked = false;
  static byte curHour = 0;
  static byte curMinute = 0;
  static byte n = 10;

  if (!tasks.getTaskState(set_time_mode))
  {
    tasks.startTask(set_time_mode);
    tasks.startTask(return_to_default_mode);
    restartBlink();
    if (displayMode > DISPLAY_MODE_ALARM_ON_OFF)
    {
      n = 0;
    }
    getData(curHour, curMinute);
    time_checked = false;
  }

  if (n < 8 && (displayMode == DISPLAY_MODE_SET_ALARM_HOUR_1 ||
                displayMode == DISPLAY_MODE_SET_ALARM_HOUR_2 ||
                displayMode == DISPLAY_MODE_SET_ALARM_INTERVAL))
  {
    n++;
    showSettingType();
    return;
  }

  if (!time_checked && (displayMode == DISPLAY_MODE_SET_HOUR ||
                        displayMode == DISPLAY_MODE_SET_MINUTE))
  {
    curHour = curTime.hour();
    curMinute = curTime.minute();
  }

  // опрос кнопок =====================
  if (btnSet.getBtnFlag() > BTN_FLAG_NONE)
  {
    if (time_checked)
    {
      saveData(curHour, curMinute);
      time_checked = false;
    }
    setDisplayMode(curHour);
    btnSet.setBtnFlag(BTN_FLAG_NONE);
  }

  if ((btnUp.getBtnFlag() == BTN_FLAG_NEXT) || (btnDown.getBtnFlag() == BTN_FLAG_NEXT))
  {
    checkSettingData(curHour, curMinute, (btnUp.getBtnFlag() == BTN_FLAG_NEXT));
    time_checked = true;
    btnUp.setBtnFlag(BTN_FLAG_NONE);
    btnDown.setBtnFlag(BTN_FLAG_NONE);
  }

  // вывод данных на экран ============
  switch (displayMode)
  {
  case DISPLAY_MODE_ALARM_ON_OFF:
    showAlarmState(curHour);
    break;
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
    showTimeData(curHour / 60, curHour % 60);
    break;
  default:
    showTimeData(curHour, curMinute);
    break;
  }
}

#ifdef USE_DS18B20
void checkDS18b20()
{
  int temp;
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);                      // Считывание значения с датчика
  temp = (ds.read() | ds.read() << 8); // Принимаем два байта температуры
  temperature = round((float)temp / 16.0);

  // даем команду на конвертацию для следующего запроса
  ds.reset();
  ds.write(0xCC); // Обращение ко всем датчикам
  ds.write(0x44); // Команда на конвертацию
}
#endif

void showTemp()
{
  if (!tasks.getTaskState(show_temp_mode))
  {
    tasks.startTask(return_to_default_mode);
    tasks.startTask(show_temp_mode);
  }

#ifdef USE_DS18B20
  disp.showTemp(temperature);
#else
  disp.showTemp(int(clock.getTemperature()));
#endif
}

void setDisp()
{
  disp.show();
}

void checkAlarm()
{
  alarm.tick(curTime);
  if (alarm.getAlarmState() == ALARM_YES && !tasks.getTaskState(alarm_buzzer))
  {
    runAlarmBuzzer();
  }
}

void runAlarmBuzzer()
{
  static byte n = 0;
  static byte k = 0;
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
  else if (alarm.getAlarmState() != ALARM_YES)
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
      alarm.setAlarmState(ALARM_ON);
    }
  }
}

#ifdef USE_LIGHT_SENSOR
void setBrightness()
{
  static uint16_t b;
  b = (b * 2 + analogRead(LIGHT_SENSOR_PIN)) / 3;
  if (b < LIGHT_THRESHOLD)
  {
    disp.setBrightness(MIN_DISPLAY_BRIGHTNESS);
  }
  else if (b > LIGHT_THRESHOLD + 50)
  {
    disp.setBrightness(MAX_DISPLAY_BRIGHTNESS);
  }
}
#endif

// ===================================================
void showTime(DateTime dt)
{
  disp.showTime(dt.hour(), dt.minute(), blink_flag);
}

void showTimeData(byte hour, byte minute)
{
  // если наступило время блинка и кнопки Up/Down не нажаты, то стереть соответствующие разряды; при нажатых кнопках Up/Down во время изменения данных ничего не мигает
  if (!blink_flag && !btnUp.isButtonClosed() && !btnDown.isButtonClosed())
  {
    switch (displayMode)
    {
    case DISPLAY_MODE_SET_HOUR:
    case DISPLAY_MODE_SET_ALARM_HOUR_1:
    case DISPLAY_MODE_SET_ALARM_HOUR_2:
      hour = -1;
      break;
    case DISPLAY_MODE_SET_MINUTE:
    case DISPLAY_MODE_SET_ALARM_MINUTE_1:
    case DISPLAY_MODE_SET_ALARM_MINUTE_2:
      minute = -1;
      break;
    case DISPLAY_MODE_SET_ALARM_INTERVAL:
      hour = -1;
      minute = -1;
      break;
    default:
      break;
    }
  }
  disp.showTime(hour, minute, (displayMode == DISPLAY_MODE_SET_ALARM_INTERVAL));
}

void showAlarmState(byte _state)
{
  disp.setDispData(0, 0b01110111); // "A"
  disp.setDispData(1, 0b10111000); // "L:"
  disp.setDispData(2, 0x00);
  if (!blink_flag && !btnUp.isButtonClosed() && !btnDown.isButtonClosed())
  {
    disp.setDispData(3, 0x00);
  }
  else
  {
    disp.setDispData(3, (_state) ? 0b01011100 : 0b00001000);
  }
}

void showSettingType()
{
  // DISPLAY_MODE_SET_ALARM_HOUR_1   - P1:
  // DISPLAY_MODE_SET_ALARM_HOUR_2   - P2:
  // DISPLAY_MODE_SET_ALARM_INTERVAL - It:
  switch (displayMode)
  {
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
    disp.setDispData(0, 0b01110011);
    disp.setDispData(1, 0b10000110);
    break;
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
    disp.setDispData(0, 0b01110011);
    disp.setDispData(1, 0b11011011);
    break;
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
    disp.setDispData(0, 0b00000110);
    disp.setDispData(1, 0b11111000);
    break;
  default:
    break;
  }
  disp.setDispData(2, 0x00);
  disp.setDispData(3, 0x00);
}

// ===================================================
void saveTime(byte hour, byte minute)
{
  clock.setHour(hour);
  clock.setMinute(minute);
  clock.setSecond(0);
}

// ===================================================
void checkData(byte &dt, byte max, bool toUp)
{
  (toUp) ? dt++ : dt--;
  if (dt > max)
  {
    dt = (toUp) ? 0 : max;
  }
}

void checkData(byte &dt, byte min, byte max, byte x, bool toUp)
{
  (toUp) ? dt += x : dt -= x;
  if (dt < min)
  {
    dt = min;
  }
  if (dt > max)
  {
    dt = max;
  }
}

// ===================================================
void setDisplay()
{
  switch (displayMode)
  {
  case DISPLAY_MODE_SET_HOUR:
  case DISPLAY_MODE_SET_MINUTE:
  case DISPLAY_MODE_SET_ALARM_HOUR_1:
  case DISPLAY_MODE_SET_ALARM_MINUTE_1:
  case DISPLAY_MODE_SET_ALARM_HOUR_2:
  case DISPLAY_MODE_SET_ALARM_MINUTE_2:
  case DISPLAY_MODE_SET_ALARM_INTERVAL:
  case DISPLAY_MODE_ALARM_ON_OFF:
    if (!tasks.getTaskState(set_time_mode))
    {
      showTimeSetting();
    }
    break;
  case DISPLAY_MODE_SHOW_TEMP:
    if (!tasks.getTaskState(show_temp_mode))
    {
      showTemp();
    }
    break;
  default:
    showTime(curTime);
    break;
  }
}

// ===================================================
void setup()
{
  // ==== часы =========================================
  Wire.begin();
  clock.setClockMode(false); // 24-часовой режим
  rtcNow();

  // ==== кнопки Up/Down ===============================
  btnUp.setLongClickMode(LCM_CLICKSERIES);
  btnUp.setIntervalOfSerial(100);
  btnDown.setLongClickMode(LCM_CLICKSERIES);
  btnDown.setIntervalOfSerial(100);

#ifdef USE_DS18B20
  // ==== датчик DS18b20 ===============================
  ds.reset();     // сброс шины
  ds.write(0x7F); // точность 0,5гр = 1F; 0,25гр = 3F; 0,125гр = 5F; 0,0625гр = 7F;
  ds.search(addr);
  checkDS18b20();
#endif

  // ==== задачи =======================================
  byte task_count = 8;
#ifdef USE_LIGHT_SENSOR
  task_count++;
#endif
#ifdef USE_DS18B20
  task_count++;
#endif
  tasks.init(task_count);

  rtc_guard = tasks.addTask(50, rtcNow);
  blink_timer = tasks.addTask(500, blink);
  return_to_default_mode = tasks.addTask(AUTO_EXIT_TIMEOUT * 1000ul, returnToDefMode, false);
  set_time_mode = tasks.addTask(100, showTimeSetting, false);
#ifdef USE_DS18B20
  ds18b20_guard = tasks.addTask(3000, checkDS18b20);
#endif
  show_temp_mode = tasks.addTask(500, showTemp, false);
  alarm_guard = tasks.addTask(200, checkAlarm);
  alarm_buzzer = tasks.addTask(50, runAlarmBuzzer, false);
  display_guard = tasks.addTask(50, setDisp);
#ifdef USE_LIGHT_SENSOR
  light_sensor_guard = tasks.addTask(100, setBrightness);
#else
  disp.setBrightness(MAX_DISPLAY_BRIGHTNESS);
#endif

  alarm.init(curTime);
}

void loop()
{
  checkButton();
  tasks.tick();
  setDisplay();
}