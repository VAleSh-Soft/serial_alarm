# serial_alarm v1.2

- [Описание](#описание)
- [Управление](#управление)
- [Будильник](#будильник)
- [Дополнительные возможности](#дополнительные-возможности)
  - [Автоматическое управление яркостью экрана](#автоматическое-управление-яркостью-экрана)
  - [Вывод на экран текущей температуры](#вывод-на-экран-текущей-температуры)
- [Подключение модулей](#подключение-модулей)
- [Использованные сторонние библиотеки](#использованные-сторонние-библиотеки)

<hr>

## Описание

Часы с возможностью выдачи сигнала будильника через равные интервалы времени в промежутке между заданным временем начала и конца сигнализации. Например, если задан промежуток с 20:00 до 05:00 с интервалом в три часа, сигнал будильника будет срабатывать в 20:00, 23:00 и 02:00. Время 05:00 является временем завершения работы и перехода будильника в неактивный режим до 20:00. Если нужно, чтобы сигнал сработал и в 05:00, время окончания нужно задать хотя бы на минуту позже - 05:01.

## Управление

Часы управляются тремя кнопками: **Set** - вход в режим настроек, выбор настраиваемого параметра и сохранение изменений; **Up** - увеличение текущих значений; **Down** - уменьшение текущих значений.

Вход в настройки текущего времени выполняется длинным кликом кнопкой **Set**.

## Будильник

Состояние будильника показывает двухцветный светодиод - если будильник включен, светодиод светится, при этом, если текущее время входит в рабочий промежуток (будильник а активном режиме), светодиод светится зеленым цветом, иначе (будильник в неактивном режиме) - красным; если будильник сработал, светодиод мигает зеленым.

Для однократного (один раз в сутки) срабатывания будильника достаточно задать одинаковое время начала и конца, величина интервала при этом настраиваться не будет.

Сигнал сработавшего будильника отключается кликом любой кнопки.

В режим настройки будильника можно перейти по двойному клику кнопкой **Set**. Включение/выключение будильника выполняется кнопками **Up** или **Down**. После включения будильника следующий клик кнопкой **Set** переводит в режим настройки времени и интервала срабатывания. 

Каждый раздел обозначается соответствующими символами: 
- **AL:** - включение/отключение будильника; 
- **Р1:** - время начала (переход будильника в активное состоиние и первое срабатывание); 
- **Р2:** - время окончания (переход будильника в неактивное состояние); 
- **It:** - интервал срабатывания (настраивается в диапазоне 10-180 минут с шагом в 10 минут);

## Дополнительные возможности

### Автоматическое управление яркостью экрана

Предусмотрена возможность снижения яркости экрана при низком освещении по данным датчика света - фоторезистора типа **GL5528**, подключенного к пину **A3**. Для этого нужно раскомментировать строку `#define USE_LIGHT_SENSOR` в файле **header_file.h**.

Схема подключения датчика:

![scheme0001](/docs/0001.jpg "Схема подключения датчика")
 

Если использование датчика света не предполагается, индикатор всегда будет работать с максимальной (т.е. заданной в строке `#define MAX_DISPLAY_BRIGHTNESS 7`) яркостью. 

### Вывод на экран текущей температуры

Для вывода на экран текущей температуры нужно раскомментировать строку `#define USE_TEMP_DATA` в файле **header_file.h**. В этом случае в режиме отображения текущего времени клик кнопкой **Up** будет выводить на несколько секунд температуру окружающей среды.

Температура по умолчанию берется из внутреннего датчика микросхемы **DS3231**, однако есть возможность использования внешнего датчика DS18b20. Для этого нужно раскомментировать строку `#define USE_DS18B20` в файле **header_file.h**.

Схема подключения датчика DS18b20:

![scheme0002](/docs/0002.jpg "Схема подключения датчика DS18b20")


## Подключение модулей

Часы построены с использованием модуля **DS3231**, семисегментного экрана  с драйвером **TM1637** и **Arduino Pro Mini** на базе **ATmega168p**. В качестве источника звука использован пассивный пьезоэлектрический излучатель. Индикаторный светодиод двухцветный, с общим катодом.

Пины для подключения экрана, модуля DS3231, кнопок, пищалки, светодиода и датчиков температуры и света определены в файле **header_file.h**

## Использованные сторонние библиотеки

**shButton.h** - https://github.com/VAleSh-Soft/shButton<br>
**shTaskManager.h** - https://github.com/VAleSh-Soft/shTaskManager<br>

Для работы с модулем **DS3231** используется библиотека<br>
**DS3231.h** - https://github.com/NorthernWidget/DS3231<br>

Для работы с экраном используется библиотека<br>
**TM1637Display.h** - https://github.com/avishorp/TM1637<br>

для работы с датчиком **DS18b20** используется библиотека<br>
**OneWire.h** - https://github.com/PaulStoffregen/OneWire

Если возникнут вопросы, пишите на valesh-soft@yandex.ru 