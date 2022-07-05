#pragma once
#include <Arduino.h>
#include <DS3231.h>
#include <EEPROM.h>

#define MAX_DATA 1439 // максимальное количество минут для установки будильника (23 ч, 59 мин)

enum IndexOffset : uint8_t // смещение от стартового индекса в EEPROM для хранения настроек
/* общий размер настроек - 7 байт */
{
  ALARM_STATE = 0,   // состояние будильника, включен/нет, uint8_t
  ALARM_POINT_1 = 1, // начало отсчета времени сигнализации в минутах от полуночи, uint16_t
  ALARM_POINT_2 = 3, // конец отсчета времени сигнализации в минутах от полуночи, uint16_t
  ALARM_INTERVAL = 5 // интервал срабатывания будильника в минутах, uint16_t
};

enum AlarmState : uint8_t // состояние будильника
{
  ALARM_OFF, // будильник выключен
  ALARM_ON,  // будильник включен
  ALARM_YES  //будильник сработал
};

class SerialAlarm
{
private:
  byte red_pin;
  byte green_pin;
  uint16_t eeprom_index;
  AlarmState state;
  uint16_t next_point;

  uint8_t
  read_eeprom_8(IndexOffset _index)
  {
    return (EEPROM.read(eeprom_index + _index));
  }

  uint16_t read_eeprom_16(IndexOffset _index)
  {
    uint16_t _data;
    EEPROM.get(eeprom_index + _index, _data);
    return (_data);
  }

  void write_eeprom_8(IndexOffset _index, uint8_t _data) { EEPROM.update(eeprom_index + _index,
                                                                         _data); }

  void write_eeprom_16(IndexOffset _index, uint16_t _data) { EEPROM.put(eeprom_index + _index,
                                                                        _data); }

  void setNextPoint(uint16_t _time)
  {
    next_point = (_time < MAX_DATA + 1) ? _time : _time - (MAX_DATA + 1);
  }

  bool checkForInterval(uint16_t _time)
  {
    if (getAlarmPoint2() == getAlarmPoint1())
    {
      return (false);
    }

    if (getAlarmPoint2() > getAlarmPoint1())
    {
      return ((_time >= getAlarmPoint1()) && (_time < getAlarmPoint2()));
    }
    else
    {
      return ((_time >= getAlarmPoint1()) || (_time < getAlarmPoint2()));
    }
  }

  void setLed(uint16_t _time)
  {
    static byte n = 0;
    byte red_state = LOW;
    byte green_state = LOW;
    if (state == ALARM_YES)
    { // при сработавшем будильнике светодиод мигает зеленым с периодом 0.2 секунды
      green_state = n;
      if (++n > 1)
      {
        n = 0;
      }
    }
    else if (state)
    {
      (checkForInterval(_time)) ? green_state = HIGH : red_state = HIGH;
    }
    digitalWrite(red_pin, red_state);
    digitalWrite(green_pin, green_state);
  }

public:
  SerialAlarm(byte _red_pin, byte _green_pin, uint16_t _eeprom_index)
  {
    red_pin = _red_pin;
    pinMode(red_pin, OUTPUT);
    green_pin = _green_pin;
    pinMode(green_pin, OUTPUT);
    eeprom_index = _eeprom_index;
    if (read_eeprom_8(ALARM_STATE) > 1)
    {
      write_eeprom_8(ALARM_STATE, 0);
    }
    if (read_eeprom_16(ALARM_POINT_1) > MAX_DATA)
    {
      write_eeprom_16(ALARM_POINT_1, 0);
    }
    if (read_eeprom_16(ALARM_POINT_1) > MAX_DATA)
    {
      write_eeprom_16(ALARM_POINT_2, 0);
    }
    if (read_eeprom_16(ALARM_INTERVAL) > 180)
    {
      write_eeprom_16(ALARM_INTERVAL, 60);
    }
    state = (AlarmState)read_eeprom_8(ALARM_STATE);
  }

  // первоначальное определение точки следующего срабатывания будильника при включении или при изменении настроек будильника
  void init(DateTime _time)
  {
    uint16_t p1 = getAlarmPoint1();
    uint16_t p2 = getAlarmPoint2();
    uint32_t tm = _time.hour() * 3600ul + _time.minute() * 60ul + _time.second();
    if (p2 < p1)
    {
      p2 += MAX_DATA + 1;
    }
    uint16_t x = p1;
    while (x * 60ul < tm)
    {
      x += getAlarmInterval();
    }
    if (!checkForInterval(x))
    {
      x = p1;
    }

    setNextPoint(x);
  }

  AlarmState getAlarmState() { return (state); }

  void setAlarmState(AlarmState _state) { state = _state; }

  bool getOnOffAlarm() { return (bool)read_eeprom_8(ALARM_STATE); }

  void setOnOffAlarm(bool _state)
  {
    write_eeprom_8(ALARM_STATE, (byte)_state);
    state = (AlarmState)_state;
  }

  uint16_t getAlarmPoint1() { return (read_eeprom_16(ALARM_POINT_1)); }

  void setAlarmPoint1(uint16_t _time) { write_eeprom_16(ALARM_POINT_1, _time); }

  uint16_t getAlarmPoint2() { return (read_eeprom_16(ALARM_POINT_2)); }

  void setAlarmPoint2(uint16_t _time) { write_eeprom_16(ALARM_POINT_2, _time); }

  uint16_t getAlarmInterval() { return (read_eeprom_16(ALARM_INTERVAL)); }

  void setAlarmInterval(byte _time)
  {
    if (_time > 180)
    {
      _time = 180;
    }
    write_eeprom_16(ALARM_INTERVAL, _time);
  }

  void tick(DateTime _time)
  {
    uint16_t tm = _time.hour() * 60 + _time.minute();
    setLed(tm);
    if (state == ALARM_ON)
    {
      if (tm * 60ul + _time.second() == next_point * 60ul)
      {
        state = ALARM_YES;
        setNextPoint(next_point + read_eeprom_16(ALARM_INTERVAL));
        if (!checkForInterval(next_point))
        {
          next_point = getAlarmPoint1();
        }
      }
    }
  }
};
