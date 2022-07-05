# serial_alarm v1.0
Часы с возможностью выдачи сигнала будильника через равные интервалы времени в промежутке между заданным временем начала и конца сигнализации. Например, если задан промежуток с 20:00 до 05:00 с интервалом в три часа, сигнал будильника будет срабатывать в 20:00, 23:00 и 02:00. Время 05:00 является временем завершения работы и перехода будильника в неактивный режим до 20:00. Если нужно, чтобы сигнал сработал и в 05:00, время окончания нужно задать хотя бы на минуту позже - 05:01.

Часы управляются тремя кнопками: **Set** - вход в режим настроек, выбор настраиваемого параметра и сохранение изменений; **Up** - увеличение текущих значений; **Down** - уменьшение текущих значений.

Вход в настройки текущего времени выполняется длинным кликом кнопкой **Set**.

## Будильник

Состояние будильника показывает светодиод - если будильник включен, светодиод светится, при этом, если текущее время входит в рабочий промежуток (будильник а активном режиме), светодиод светится зеленым цветом, иначе (будильник в неактивном режиме) - красным; если будильник сработал, светодиод мигает зеленым.

Для однократного (один раз в сутки) срабатывания будильника достаточно задать одинаковое время начала и конца, величина интервала при этом настраиваться не будет.

Сигнал сработавшего будильника отключается кликом любой кнопки.

В режим настройки будильника можно перейти по двойному клику кнопкой **Set**. Включение/выключение будильника выполняется кнопками **Up** или **Down**. После включения будильника следующий клик кнопкой **Set** переводит в режим настройки времени и интервала срабатывания. Каждый раздел обозначается соответствующими символами: **AL:** - включение/отключение будильника, **Р1:** - время начала (переход будильника в активное состоиние и первое срабатывание), **Р2:** - время окончания (переход будильника в неактивное состояние), **It:** - интервал срабатывания (настраивается в диапазоне 10-180 минут с шагом в 10 минут).

## Дополнительные возможности

### Автоматическое управление яркостью экрана

Предусмотрена возможность снижения яркости экрана при низком освещении по данным датчика света - фоторезистора типа **GL5528**, подключенного к пину **A3**. Для этого нужно раскомментировать строку `#define USE_LIGHT_SENSOR` в файле **header_file.h**.

Схема подключения датчика: `GND -> R(10k) -> A3 <- Rl <- VCC`, где **R** - резистор 10кОм, **Rl** - фоторезистор. 

Если использование датчика света не предполагается, индикатор всегда будет работать с максимальной (т.е. заданной в строке `#define MAX_DISPLAY_BRIGHTNESS 7`) яркостью. 

### Вывод на экран текущей температуры

Для вывода на экран текущей температуры нужно раскомментировать строку `#define USE_TEMP_DATA // использовать или нет вывод температуры по клику кнопкой Up` в файле **header_file.h**. В этом случае в режиме отображения текущего времени клик кнопкой **Up** будет выводить на несколько секунд температуру окружающей среды.

Температура по умолчанию берется из внутреннего датчика микросхемы **DS3231**, однако есть возможность использования внешнего датчика DS18b20. Для этого нужно раскомментировать строку `#define USE_DS18B20` в файле **header_file.h**.

### Подключение модулей

Часы построены с использованием модуля **DS3231**, семисегментного экрана  с драйвером **TM1637** и **Arduino Pro Mini** на базе **ATmega168p**. В качестве источника звука использована пассивный пьезоэлектрический излучатель. Индикаторный светодиод двухцветный, с общим катодом.

Пины для подключения экрана, кнопок, пищалки, светодиода и датчиков температуры и света определены в файле **header_file.h**

Модуль DS3231 подключен к пинам A4 (SDA) и A5 (SCL)

Если возникнут вопросы, пишите на valesh-soft@yandex.ru 