#pragma once
#include <Arduino.h>
#include <TM1637Display.h> // https://github.com/avishorp/TM1637

// ==== класс для вывода данных на экран =============
class DisplayTM1637 : public TM1637Display
{
private:
  byte data[4];
  byte _brightness = 1;

public:
  DisplayTM1637(uint8_t clk_pin, uint8_t dat_pin) : TM1637Display(clk_pin, dat_pin)
  {
    clear();
  }

  /**
   * @brief очистка буфера экрана, сам экран при этом не очищается
   *
   */
  void clear();

  /**
   * @brief очистка экрана
   *
   */
  void sleep();

  /**
   * @brief установка разряда _index буфера экрана
   *
   * @param _index разряд буфера
   * @param _data данные для установки
   */
  void setDispData(byte _index, byte _data);

  /**
   * @brief получение значения разряда _index буфера экрана
   *
   * @param _index разряд буфера
   * @return byte
   */
  byte getDispData(byte _index);

  /**
   * @brief отрисовка на экране содержимого его буфера
   *
   */
  void show();

  /**
   * @brief вывод на экран  времени; если задать какое-то из значений hour или minute отрицательным, эта часть экрана будет очищена - можно организовать мигание, например, в процессе настройки времени
   *
   * @param hour часы
   * @param minute минуты
   * @param show_colon отображать или нет двоеточие между часами и минутами
   */
  void showTime(int8_t hour, int8_t minute, bool show_colon);

  /**
   * @brief вывод на экран температуры в диапазоне от -99 до +99 градусов; вне диапазона выводится строка минусов
   *
   * @param temp данные для вывода
   */
  void showTemp(int temp);

  /**
   * @brief вывод на экран данных по настройке яркости экрана
   *
   * @param br величина яркости
   * @param blink используется для мигания изменяемого значения
   * @param toSensor используется или нет датчик освещенности
   * @param toMin если true, то настраивается минимальный уровень яркости, иначе - максимальный
   */
  void showBrightnessData(byte br, bool blink, bool toSensor = false, bool toMin = false);

  /**
   * @brief установка яркости экрана; реально яркость будет изменена только после вызова метода show()
   *
   * @param brightness значение яркости (1..7)
   */
  void setBrightness(byte brightness);
};

// ---- public ----------------------------------

;  void DisplayTM1637::clear()
  {
    for (byte i = 0; i < 4; i++)
    {
      data[i] = 0x00;
    }
  }

  void DisplayTM1637::sleep()
  {
    clear();
    TM1637Display::setSegments(data);
  }

  void DisplayTM1637::setDispData(byte _index, byte _data)
  {
    if (_index < 4)
    {
      data[_index] = _data;
    }
  }

  byte DisplayTM1637::getDispData(byte _index)
  {
    return ((_index < 4) ? data[_index] : 0);
  }

  void DisplayTM1637::show()
  {
    bool flag = false;
    static byte _data[4] = {0x00, 0x00, 0x00, 0x00};
    static byte br = 0;
    for (byte i = 0; i < 4; i++)
    {
      flag = _data[i] != data[i];
      if (flag)
      {
        break;
      }
    }
    if (!flag)
    {
      flag = br != _brightness;
    }
    // отрисовка экрана происходит только если изменился хотя бы один разряд или изменилась яркость
    if (flag)
    {
      for (byte i = 0; i < 4; i++)
      {
        _data[i] = data[i];
      }
      br = _brightness;
      TM1637Display::setSegments(data);
    }
  }

  void DisplayTM1637::showTime(int8_t hour, int8_t minute, bool show_colon)
  {
    clear();
    if (hour >= 0)
    {
      data[0] = TM1637Display::encodeDigit(hour / 10);
      data[1] = TM1637Display::encodeDigit(hour % 10);
    }
    if (minute >= 0)
    {
      data[2] = TM1637Display::encodeDigit(minute / 10);
      data[3] = TM1637Display::encodeDigit(minute % 10);
    }
    if (show_colon)
    {
      data[1] |= 0x80; // для показа двоеточия установить старший бит во второй цифре
    }
  }

  void DisplayTM1637::showTemp(int temp)
  {
    clear();
    data[3] = 0x63;
    // если температура отрицательная, сформировать минус впереди
    if (temp < 0)
    {
      temp = -temp;
      data[1] = 0x40;
    }
    // если температура выходит за диапазон, сформировать строку минусов
    if (temp > 99)
    {
      for (byte i = 0; i < 4; i++)
      {
        data[i] = 0x40;
      }
    }
    else
    {
      if (temp > 9)
      {
        if (data[1] == 0x40)
        { // если температура ниже -9, переместить минус на крайнюю левую позицию
          data[0] = 0x40;
        }
        data[1] = TM1637Display::encodeDigit(temp / 10);
      }
      data[2] = TM1637Display::encodeDigit(temp % 10);
    }
  }

  void DisplayTM1637::showBrightnessData(byte br, bool blink, bool toSensor = false, bool toMin = false)
  {
    clear();
    data[0] = 0b01111100;
    if (toSensor)
    {
      data[1] = (toMin) ? encodeDigit(0) : encodeDigit(1);
    }
    else
    {
      data[1] = 0b01010000;
    }
    data[1] |= 0x80; // для показа двоеточия установить старший бит во второй цифре

    if (!blink)
    {
      data[2] = encodeDigit(br / 10);
      data[3] = encodeDigit(br % 10);
    }
  }

   void DisplayTM1637::setBrightness(byte brightness)
  {
    _brightness = (brightness <= 7) ? brightness : 7;
    TM1637Display::setBrightness(brightness, true);
  }
