// Переделана работа кнопок, нет нажатий двух кнопок одновременно,
// в главном меню при выборе функции и удержании кнопки 2 настраиваем:
// В вольтметре - корреция напряжения
// в осциллографе - тестовый генератор
// в частотометре - выключение пищалки
// в генераторе - смена полярности выхода
// в канлинтесте - переворот экрана
// в дпкв(кранк сенсор) - включение фонарика 4 пин
// убран зеркальный шим в генераторе
// перенесена коррекция напряжения
#include "EEPROM.h"  // подклчаем библиотеку работы с епром
#include "U8g2lib.h" // подключаем библиотеку работы с дисплеем
#include "_pins.h"   // настройка  для разных платформ

void correction();
void voltmetr();
void oscilograf();
void shastotomer();
void generator();
void can_lin_test();
void timers();
void dpkv();
void drawScreen(uint32_t duration);

#define DEBUG 0
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define log Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#define log(x)
#endif

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA); // дисплей
byte buffer[128];                                                                                              // буфер для осциллографа
unsigned long timer;                                                                                           // переменная для таймера
const PROGMEM char caption_voltmeter[] = "VOLTMETER";                                                          // складываем строки во флеш память освобождая оперативку
const PROGMEM char caption_oscilloscope[] = "OSCILLOSCOPE";
const PROGMEM char caption_freq[] = "FREQUENCE";
const PROGMEM char caption_generator[] = "GENERATOR";
const PROGMEM char caption_canlintst[] = "CAN LIN TEST";
const PROGMEM char caption_timer[] = "TIMER";
const PROGMEM char caption_dpkv[] = "CRANK SENSOR";
bool x = 0;           // полярность + - выхода сигнала с контрольки
bool j = 0;           // вкл - выкл фонарик
bool d = 0;           // старт - стоп для дпкв
bool p = 0;           // подключаем - отключаем постоянный плюс на иглу в генераторе
bool f = 0;           // поворот - переворот экрана
bool u = 0;           // вход - запрет в главное меню
bool r = 0;           // быстрый - медленный вход в функцию
byte led = 10;        // пин генератора 10 - минус по умолчанию, 9 пин генератор +
int frequency = 30;   // начальная частота  на генераторе
byte brightness = 25; // начальная частота ШИМ (RX_LED_pin-255) = 10%
byte k = 10;          // на генераторе шим отображает
int y = 0;            // частота тестового сигнала
byte jdem;            // расходник для таймера время ожидания
byte impyls;          // расходник для таймера время подачи
byte n = 1;           // настройка для таймера время ожидания,  по умолчанию 1 сек
byte g = 1;           // настройка для таймера время подачи,  по умолчанию 1сек
byte s = 16;          // пин А2(16) меняется на другой для отключения пищалки
byte b = 0;           // счетчик для определения удержания кнопок
byte i = 58;          // дпкв импульсы, стандартные 60 - 2 по умолчанию
byte a = 2;           // дпкв пропуски, стандартные 60 - 2 по умолчанию
byte t = 0;           // храним после считывания значение из епром и пишем в епром в функции кор
byte h = 0;           // время развертки осцилограф
byte q = 1;           // счетчик перемещения по меню
byte e = 0;           // хранилище номера функции на быстром входе
byte m = 5;           // вольты на осцилографе
int w = 100;          // высота осциллографа
int Htime = 0;        // переменная целого типа для хранения длительности высокого уровня
int Ltime = 0;        // переменная целого типа для хранения длительности низкого уровня
int freq;             // переменная для хранения значения частоты
int pwm;              // переменная для хранения значения частоты
float Ttime;          // переменная для хранения длительности периода
float rtime;          // переменная для хранения длительности периода
float proc;           // переменная для хранения значения частоты

const int ledChannel = 0;  // PWM channel, can be from 0 to 15
const int ledPin = PIN_D7; // Pin to which the device is connected
                           // int frequency  = 5000;  // Frequency in Hz
const int resolution = 8;  // Resolution in bits (from 1 to 15)

void setup()
{ ///////////// Н А С Т Р О Й К А (сетап) ////////////////////////////////

  // t = EEPROM.read(0);         // сохраняем в переменную хначение из епром

  ledcSetup(ledChannel, frequency, resolution);
  ledcAttachPin(ledPin, ledChannel);
  // InitTimersSafe();           // запускаем таймер для ШИМ

  pinMode(ADC_pin, INPUT);        // пин а7 вольтметр и осцилограф
  pinMode(CLC_pin, INPUT_PULLUP); // пин 2 вход данных частотомер

  pinMode(KEY_3_pin, INPUT_PULLUP); // 3 кнопка
  pinMode(KEY_2_pin, INPUT_PULLUP); // 2 кнопка
  pinMode(KEY_1_pin, INPUT_PULLUP); // 1 кнопка

  pinMode(BEEP_pin, OUTPUT); // пин а0 пищалка+++
  pinMode(GEN0_pin, OUTPUT); // пин 10 выход на генератор -
  pinMode(GEN1_pin, OUTPUT); // пин 9 выход на генератор + +++++++

  pinMode(RX_LED_pin, OUTPUT);    // светодиод на плате тх
  pinMode(TX_LED_pin, OUTPUT);    // светодиод а плате рх
  pinMode(LED_pin, OUTPUT);       // пин 13 светодиод а плате
  digitalWrite(TX_LED_pin, HIGH); // выключаем изначально светордиод рх
  u8g2.begin();                   // Инициализируем дисплей
  //  u8g2.setDisplayRotation(U8G2_R2);  // преворачиваем изображение изначально (если необходимо)
  u8g2.setFont(u8g2_font_10x20_mr); // Выставляем шрифт (шрифты жрут прорву памяти так что аккуратнее если меняете)
  u8g2.setCursor(15, 13);           // определяем место на дисплее
  u8g2.print("KONTROLKA");          // пишем текст
  u8g2.setCursor(15, 32);
  u8g2.print("V 1. 10. 2.");
  u8g2.sendBuffer();              // Отсылаем данные на дисплей
  tone(BEEP_pin, 349, 204.54525); // Проигрываем мелодию
  delay(227.2725);
  tone(BEEP_pin, 349, 204.54525);
  delay(227.2725);
  tone(BEEP_pin, 349, 102.272625);
  delay(113.63625);
  tone(BEEP_pin, 391, 102.272625);
  delay(113.63625);
  tone(BEEP_pin, 329, 204.54525);
  delay(227.2725);
  tone(BEEP_pin, 329, 204.54525);
  delay(227.2725);
  tone(BEEP_pin, 293, 102.272625);
  delay(113.63625);
  tone(BEEP_pin, 261, 102.272625);
  delay(113.63625);
  tone(BEEP_pin, 293, 306.817875); // конец мелодии
}
void loop()
{ ////// Г Л А В Н Ы Й  Ц И К Л  П Р О Г Р А М М Ы ///////////////////////////////////
  ///////////////////////// К Н О П К И //////////////////////////
  ///////////////// главное меню 1 кнопка перемещение вправо /////////////////////////////
  if (digitalRead(KEY_1_pin) == 0)
  { // 1 верхняя кнопка, обработка нажатия
    u8g2.setFont(u8g2_font_10x20_mr);
    ledcWrite(led, 0);
    // SetPinFrequency(led, 0);
    digitalWrite(GEN1_pin, 0);      // отключаем на всякий случай питание постоянное этого пина
    digitalWrite(GEN0_pin, 0);      // отключаем на всякий случай питание постоянное этого пина
    digitalWrite(TX_LED_pin, HIGH); // Гасим светодиоды тх rx
    digitalWrite(RX_LED_pin, HIGH); // Гасим светодиоды тх rx
    b++;                            // счетчик для удержания кнопки
    h = 0;                          // обнуляем переменные
    u = 0;
    p = 0;
    r = 0;
    e = 0;
    timer = millis(); // Из таймера скидываем данные в переменную (для задержек без delay)
    delay(300);       // задержка 300 милисекунд для сработки кнопки
    if (digitalRead(KEY_1_pin) == 1)
    {                    // отпускание кнопки в главном меню
      tone(s, 500, 100); // писк при отпускании кнопки
      q++;               // прибавляем счетчик меню
      b = 0;             // обнуляем счетчик удержания
    }
    if (q >= 8)
      q = 1; // меню возращаемся к 1 пункту
  }
  /////////////// главное меню 3 кнопка переход по меню влево /////////////////////////////
  if (r == 0 && digitalRead(KEY_3_pin) == 0)
  {
    u = 0;
    b++;
    delay(300);
    if (digitalRead(KEY_3_pin) == 1)
    {
      tone(s, 500, 100);
      q--;
      b = 0;
    }
    if (q <= 0)
      q = 7; // меню возращаемся к последнему пункту
    timer = millis();
  }
  ////////// главное меню и вольтметр 2 кнопка, выключаем пищалку удержание 3 сек //////
  if (r == 0 && q == 1 && digitalRead(KEY_2_pin) == 0)
  {
    b++; // счетчик для понимания удерживается ли кнопка
    if (b >= 4)
    {
      digitalWrite(GEN1_pin, 1);
      tone(s, 500, 100);
      delay(1000);
      correction(); // включаем функцию коррекция напряжения вольтметра
      b = 0;
    }
    timer = millis();
    u = 1;
  }
  ////////// главное меню и осциллограф 2 кнопка, тестовый генератор удержание 3 сек //////
  if (r == 0 && q == 2 && digitalRead(KEY_2_pin) == 0)
  {
    b++; // счетчик для понимания удерживается ли кнопка
    if (y == 0 && b >= 4)
    {
      u8g2.firstPage();
      u8g2.setCursor(0, 13);
      y = 35;
      tone(GEN1_pin, y, 300); // на 9 пине создаем сигнал 35 герц
      u8g2.print("TEST 35 Hz");
      b = 0;
    }
    if (y == 35 && b >= 4)
    {
      u8g2.firstPage();
      u8g2.setCursor(0, 13);
      y = 700;
      tone(GEN1_pin, y, 300); // на 9 пине создаем сигнал 700 герц
      u8g2.print("TEST 700 Hz");
      b = 0;
    }
    if (y == 700 && b >= 4)
    {
      u8g2.firstPage();
      u8g2.setCursor(0, 13);
      y = 0;
      noTone(GEN1_pin);
      tone(s, 700, 100);
      u8g2.print("TEST 0 Hz");
      b = 0;
    }
    u8g2.sendBuffer();
    delay(1000);
    timer = millis();
    u = 0;
  }
  ////////// главное меню и частотомер 2 кнопка, выключаем пищалку удержание 3 сек //////
  if (r == 0 && q == 3 && digitalRead(KEY_2_pin) == 0)
  {
    b++; // счетчик для понимания удерживается ли кнопка
    if (s == 16 && b >= 4)
    {
      u8g2.firstPage();
      u8g2.setCursor(0, 13);
      s = 8; // ВЫКЛЮЧЕНЕ ПИЩАЛКИ ПРОИЗВОДИТСЯ ПЕРЕОПРЕДЕЛЕНИЕМ НА НЕЗАДЕЙСТВОВАННЫЙ ПИН (поменяйте если задействован)
      u8g2.print("TONE OFF");
      b = 0;
    }
    if (s == 8 && b >= 4)
    {
      u8g2.firstPage();
      u8g2.setCursor(0, 13);
      s = 16;
      tone(s, 700, 100);
      u8g2.print("TONE ON");
      b = 0;
    }
    u8g2.sendBuffer();
    delay(1000);
    timer = millis();
    u = 0;
  }
  ////////// главное меню и генератор 2 кнопка, меняем полярность выходного сигнала удержание 3 сек //////
  if (r == 0 && q == 4 && digitalRead(KEY_2_pin) == 0)
  {
    b++; // счетчик для понимания удерживается ли кнопка
    if (x == 0 && b >= 4)
    {
      u8g2.firstPage();
      u8g2.setCursor(0, 13);
      tone(s, 700, 100);
      x = 1;
      led = 9; // переназначение генерации на 9 пин
      u8g2.print("+ EXIT");
      b = 0;
    }
    if (x == 1 && b >= 4)
    {
      u8g2.firstPage();
      u8g2.setCursor(0, 13);
      tone(s, 700, 100);
      x = 0;
      led = 10; // переназначение генерации на 10 пин
      u8g2.print("- EXIT");
      b = 0;
    }
    u8g2.sendBuffer();
    delay(1000);
    timer = millis();
    u = 0;
  }
  /////////////////// главное меню и канлинтест 2 кнопка, переворот экрана удержание 3 сек //////////////////////
  if (r == 0 && q == 5 && digitalRead(KEY_2_pin) == 0)
  {
    b++; // счетчик для понимания удерживается ли кнопка
    if (f == 0 && b >= 4)
    {
      tone(s, 700, 100);
      f = 1;
      u8g2.setDisplayRotation(U8G2_R2); // переворачиваем экран
      b = 0;
    }
    if (f == 1 && b >= 4)
    {
      tone(s, 700, 100);
      f = 0;
      u8g2.setDisplayRotation(U8G2_R0); // переворачиваем экран обратно
      b = 0;
    }
    delay(1000);
    if (digitalRead(KEY_2_pin) == 1)
    {
      b = 0;
    }
    timer = millis();
    u = 0;
  }

  /////////////////// главное меню и дпкв 2 кнопка, фонарь удержание 3 сек //////////////////////
  if (r == 0 && q == 7 && digitalRead(KEY_2_pin) == 0)
  {
    b++; // счетчик для понимания удерживается ли кнопка
    if (j == 0 && b >= 4)
    {
      tone(s, 700, 100);
      j = 1;
      digitalWrite(4, 1); // на 4 пине включаем +
      b = 0;
    }
    if (j == 1 && b >= 4)
    {
      tone(s, 700, 100);
      j = 0;
      digitalWrite(4, 0); //  4 пин выключаем
      b = 0;
    }
    delay(1000);
    timer = millis();
    u = 0;
  }
  if (j == 1 && millis() - timer > 30000)
  { //  """""""4 пин выключаем после 30 секунд бездействия фонарь"""""
    digitalWrite(4, 0);
    j = 0;
  }

  /////////////// режим вольтметр 2 кнопка плюс на иглу ///////////////////////
  if (e == 2 && digitalRead(KEY_2_pin) == 0)
  {
    digitalWrite(GEN0_pin, 0); // выключаем подачу минуса на иглу на всякий случай
    tone(s, 700, 100);
    if (digitalRead(GEN1_pin) == 0)
      digitalWrite(GEN1_pin, 1); // включаем подачу плюса на иглу
    else
      digitalWrite(GEN1_pin, 0); // выключаем подацу плюса на иглу
    delay(500);
  }
  ////////////////  режим вольтметр 3 кнопка подключение нагрузки  //////////////////////////
  if (e == 2 && digitalRead(KEY_3_pin) == 0)
  {
    digitalWrite(GEN1_pin, 0); // выключаем подачу плюса на иглу на всякий случай
    tone(s, 700, 100);
    if (digitalRead(GEN0_pin) == 0)
      digitalWrite(GEN0_pin, 1); // включаем подачу минуса на иглу
    else
      digitalWrite(GEN0_pin, 0); // выклюаем подачу минуса на иглу
    delay(500);
  }
  ////////////// режиме осциллограф 2 кнопка вольтаж  /////////////////////////////
  if (e == 3 && digitalRead(KEY_2_pin) == 0)
  {
    b++;
    delay(500);
    if (digitalRead(KEY_2_pin) == 1)
    {
      tone(s, 500, 100);
      m = m + 10;
      b = 0;
    }

    if (b >= 3 && digitalRead(GEN1_pin) == 0)
    { //   УДЕРЖАНИЕ КНОПКИ
      digitalWrite(GEN1_pin, 1);
      tone(s, 2000, 300);
      delay(500);
      b = 0;
    }
    if (b >= 3 && digitalRead(GEN1_pin) == 1)
    {
      digitalWrite(GEN1_pin, 0);
      tone(s, 500, 300);
      delay(500);
      b = 0;
    }
    if (m >= 36)
      m = 5; // выставляем различный диапазон напряжений на осцилоографе
    if (m == 5)
      w = 100;
    if (m == 15)
      w = 300;
    if (m == 25)
      w = 500;
    if (m == 35)
      w = 700;
    timer = millis();
  }
  ///////////////// режим осцилографа 3 кнопка время развертки //////////////////////////////
  if (e == 3 && digitalRead(KEY_3_pin) == 0)
  {      // если мы в осциллографе и нажата кнопка 3
    h++; // прибавляем при нажатии время развертки
    if (h >= 3)
      h = 0;
    u8g2.setCursor(118, 13);
    tone(s, 700, 100);
    u8g2.print(h);
    u8g2.sendBuffer();
    delay(500);
  }
  /////// режим частотомер нет настроек  //////////////////////////////////////////////

  //////////////  режиме генератор 2 кнопка скважность /////////////////////////////
  if (e == 5 && digitalRead(KEY_2_pin) == 0)
  {
    tone(s, 700, 100);
    b++;
    if (b >= 3)
      k = k + 5; // прибавляем по 5 к таймеру при удержании кнопки
    else
      k++;
    if (k >= 100)
      k = 1;
    brightness = (255 / 100) * k; // переводим проценты в в реальный ШИМ (RX_LED_pin-255)
    delay(500);
    if (digitalRead(KEY_2_pin) == 1)
      b = 0; // обнуляем счетчик удержания кнопки
  }
  //////////////  режим генератор 3 кнопка частота /////////////////////////////
  if (e == 5 && digitalRead(KEY_3_pin) == 0)
  {
    tone(s, 700, 100);
    b++;
    if (b >= 3)
      frequency = frequency + 100; // прибавляем по 100  при удержании кнопки
    else
      frequency = frequency + 10; // прибавляем по 10 нажатии кнопки
    if (frequency >= 2000)
      frequency = 0;
    delay(500);
    if (digitalRead(KEY_3_pin) == 1)
      b = 0;
  }
  //////////////  режим кан лин тест нет настроек ////////////////////////////
  ///////////////////// режим таймер 2 кнопка, время ожидания //////////////////////////
  if (e == 7 && digitalRead(KEY_2_pin) == 0)
  {
    tone(s, 700, 100);
    b++;
    if (b >= 3)
      g = g + 5; // прибавляем по 5 к таймеру при удержании кнопки
    else
      g++; // если нет удержания то добавляем 1 к таймеру
    if (g >= 99)
      g = 1; // скидываем таймер на 1 если 99 на счетчике
    delay(500);
    if (digitalRead(KEY_2_pin) == 1)
      b = 0;
  }
  ////////////////////  режим таймер 3 кнопка, время подачи //////////////////////////
  if (e == 7 && digitalRead(KEY_3_pin) == 0)
  {
    tone(s, 700, 100);
    b++;
    if (b >= 3)
      n = n + 5; // прибавляем по 5 к таймеру при удержании кнопки
    else
      n++;
    if (n >= 99)
      n = 1;
    delay(500);
    if (digitalRead(KEY_3_pin) == 1)
      b = 0;
  }
  //////////////////////// Режим дпкв 2 кнопка количество импульсов  //////////////////
  if (e == 8 && digitalRead(KEY_2_pin) == 0)
  {
    tone(s, 700, 100);
    b++; // счетчик для понимания удерживается ли кнопка
    if (b >= 3)
      i = i + 5; // прибавляем по 5 при удержании
    else
      i++;
    if (i >= 99)
      i = 1;
    delay(500);
    timer = millis();
    if (digitalRead(KEY_2_pin) == 1)
      b = 0;
  }
  //////////////////// режим дпкв 3 кнопка пропуски ////////////////////////////
  if (e == 8 && digitalRead(KEY_3_pin) == 0)
  {
    tone(s, 700, 100);
    b++;
    if (b >= 3)
      a = a + 5; // прибавляем по 5 к таймеру при удержании кнопки
    else
      a++;
    if (a >= 99)
      a = 1;
    delay(500);
    timer = millis();
    if (digitalRead(KEY_3_pin) == 1)
      b = 0;
  }
  //////////////////// режим kor  2 кнопка (внутри функции коррекция)////////////////////////////
  if (e == 9 && digitalRead(KEY_2_pin) == 0)
  {
    tone(s, 2000, 500);
    // EEPROM.update(RX_LED_pin, t);  // сохраняем значение в епром память ардуино
    // EEPROM.
    delay(500);
  }
  //////////////////////// Режим kor 3 кнопка  (внутри функции коррекция) //////////////////
  if (e == 9 && digitalRead(KEY_3_pin) == 0)
  {
    tone(s, 700, 100);
    t++; // при нажатии увеличиваем коррекцию на 1
    delay(500);
    if (t == 25)
      t = 0; // Обнуляем если набрали 25 в коррекции
  }
  ///////////////////////////////////////  Г Л А В Н О Е  М Е Н Ю ////////////////////////////////////////////////
  /////////////////////// ВОЛЬТМЕР
  if (u == 0 && q == 1)
  {                                            // если выбрали вольтметр из первой и третьей кнопки
    u8g2.clearBuffer();                        // очищаем память дисплея
    u8g2.setCursor(0, 13);                     // выставляем позицию строки
    char buf[strlen_P(caption_voltmeter) + 1]; // вытаскиваем из флеша строку вольтметр
    strcpy_P(buf, caption_voltmeter);          // вытаскиваем из флеша строку вольтметр
    u8g2.print(buf);                           // пишем на экране то что достали
    u8g2.drawCircle(5, 25, 5, U8G2_DRAW_ALL);  // рисуем пустотелый круг
    u8g2.drawCircle(24, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(43, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(62, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(81, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(100, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(119, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawDisc(5, 25, 3, U8G2_DRAW_ALL); // рисуем заполненный круг
    u8g2.sendBuffer();                      // отправляем все в дисплей
    timer = millis();
    u = 1; // выставляем значения для перехода через ожидание в функцию
    r = 0;
  }
  /////////////////////// ОСЦИЛОГРАФ
  if (u == 0 && q == 2)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 13);
    char buf[strlen_P(caption_oscilloscope) + 1];
    strcpy_P(buf, caption_oscilloscope);
    u8g2.print(buf);
    u8g2.drawCircle(5, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(24, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(43, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(62, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(81, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(100, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(119, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawDisc(24, 25, 3, U8G2_DRAW_ALL);
    u8g2.sendBuffer();
    timer = millis();
    u = 1;
    r = 0;
  }
  /////////////////////// ЧАСТОТОМЕР
  if (u == 0 && q == 3)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 13);
    char buf[strlen_P(caption_freq) + 1];
    strcpy_P(buf, caption_freq);
    u8g2.print(buf);
    u8g2.drawCircle(5, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(24, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(43, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(62, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(81, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(100, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(119, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawDisc(43, 25, 3, U8G2_DRAW_ALL);
    u8g2.sendBuffer();
    timer = millis();
    u = 1;
    r = 0;
  }
  /////////////////////// ГЕНЕРАТОР
  if (u == 0 && q == 4)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 13);
    char buf[strlen_P(caption_generator) + 1];
    strcpy_P(buf, caption_generator);
    u8g2.print(buf);
    u8g2.drawCircle(5, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(24, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(43, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(62, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(81, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(100, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(119, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawDisc(62, 25, 3, U8G2_DRAW_ALL);
    u8g2.sendBuffer();
    timer = millis();
    u = 1;
    r = 0;
  }
  /////////////////////// CAN LIN TEST
  if (u == 0 && q == 5)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 13);
    char buf[strlen_P(caption_canlintst) + 1];
    strcpy_P(buf, caption_canlintst);
    u8g2.print(buf);
    u8g2.drawCircle(5, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(24, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(43, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(62, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(81, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(100, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(119, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawDisc(81, 25, 3, U8G2_DRAW_ALL);
    u8g2.sendBuffer();
    timer = millis();
    u = 1;
    r = 0;
  }
  /////////////////////// TIMER
  if (u == 0 && q == 6)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 13);
    char buf[strlen_P(caption_timer) + 1];
    strcpy_P(buf, caption_timer);
    u8g2.print(buf);
    u8g2.drawCircle(5, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(24, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(43, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(62, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(81, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(100, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(119, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawDisc(100, 25, 3, U8G2_DRAW_ALL);
    u8g2.sendBuffer();
    timer = millis();
    u = 1;
    r = 0;
    ;
  }
  /////////////////////// DPKV
  if (u == 0 && q == 7)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 13);
    char buf[strlen_P(caption_dpkv) + 1];
    strcpy_P(buf, caption_dpkv);
    u8g2.print(buf);
    u8g2.drawCircle(5, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(24, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(43, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(62, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(81, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(100, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawCircle(119, 25, 5, U8G2_DRAW_ALL);
    u8g2.drawDisc(119, 25, 3, U8G2_DRAW_ALL);
    u8g2.sendBuffer();
    timer = millis();
    u = 1;
    r = 0;
    ;
  }
  ////////////////////////////// В К Л Ю Ч Е Н И Е  Ф У Н К Ц И Й ////////////////////////////////
  ///////////вольтметр
  if (r == 1 && e == 2)
  {             // короткий путь до вольтметра
    voltmetr(); // включаем функцию вольтметр
  }
  if (millis() - timer > 3000 && q == 1 && r == 0)
  { // если выбран вольтметр ждем 3 секунды перед входом
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    timer = millis();
    voltmetr();
  }
  ////////////////осциллограф
  if (r == 1 && e == 3)
  { // короткий путь до осцилограф
    oscilograf();
  }
  if (millis() - timer > 3000 && q == 2 && r == 0)
  { // если выбран осцилограф
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    timer = millis();

    oscilograf();
  }
  //////////////// частотомер
  if (r == 1 && e == 4)
  { // короткий путь до частотомер
    shastotomer();
  }
  if (millis() - timer > 3000 && q == 3 && r == 0)
  { // если выбран частотомер
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    timer = millis();

    shastotomer();
  }
  ////////////////  генератор
  if (r == 1 && e == 5)
  { // короткий путь до генератор
    generator();
  }
  if (millis() - timer > 3000 && q == 4 && r == 0)
  { // если выбран генератор
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    timer = millis();
    y = 0;
    noTone(GEN1_pin);
    generator();
  }
  ////////////////  кан лин тест
  if (r == 1 && e == 6)
  { // короткий путь до кан лин тест
    can_lin_test();
  }
  if (millis() - timer > 3000 && q == 5 && r == 0)
  { // если выбран кан лин тест
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    timer = millis();
    y = 0;
    noTone(GEN1_pin);
    can_lin_test();
  }
  ////////////////  таймер
  if (r == 1 && e == 7)
  { // короткий путь до таймер
    timers();
  }
  if (millis() - timer > 2000 && q == 6 && r == 0)
  { // если выбран таймер
    jdem = n + 1;
    impyls = g;
    timer = millis();
    y = 0;
    noTone(GEN1_pin);
    timers();
  }
  //////////////// дпкв
  if (r == 1 && e == 8)
  { // короткий путь до дпкв
    dpkv();
  }
  if (millis() - timer > 3000 && q == 7 && r == 0)
  { // если выбран дпкв
    timer = millis();
    y = 0;            // выключаем проверочный генератор
    noTone(GEN1_pin); // выключаем проверочный генератор
    dpkv();
  }
  //////////////// correction
  if (r == 1 && e == 9)
  { // короткий путь до kor
    correction();
  }
} ///////////////////////КОНЕЦ ГЛАВНОГО ЦИКЛА ПРОГРАММЫ //////////////////////////////
/////////////////////////////// Ф У Н К Ц И И //////////////////////////////////////////////
/////////// функция частотомер
void shastotomer()
{
  Ttime = Htime + Ltime;  // вычисляем время за сколько проходит верх и низ волны в микросекундах
  rtime = Ttime - Ltime;  // вычисляем разницу верха и низа волны для шим в микросекундах
  proc = Ttime / 100;     // Общее время верха и низа волны делим на 100 для процентов
  pwm = rtime / proc;     // вычисляем шим в процентах
  freq = 1000000 / Ttime; // вычисляем значение частоты учитывая что время у нас в микросекундах
  u8g2.firstPage();
  u8g2.setCursor(0, 13);
  u8g2.print("PWM  Hz     ");
  u8g2.setCursor(0, 32);
  u8g2.print(pwm);
  u8g2.print("   ");
  u8g2.print(freq);
  u8g2.sendBuffer();
  if (millis() - timer > 1000)
  {                                      // считывае сигнал если прошла секунда для лучшего выхода из функции
    Htime = pulseIn(CLC_pin, HIGH, 500); // считываем длительность высокого уровня
    Ltime = pulseIn(CLC_pin, LOW, 500);  // считываем длительность низкого уровня
    timer = millis();
  }
  r = 1; // для быстрого входа в функцию выставляем значения
  e = 4;
}
//////////////////////// функция генератор
void generator()
{
  u8g2.firstPage();
  u8g2.setCursor(0, 13);
  u8g2.print(k);
  if (led == 9)
    u8g2.print("  PWM +");
  else
    u8g2.print("  PWM -");
  if (p == 1)
    u8g2.print(" +");
  u8g2.setCursor(0, 32);
  u8g2.print(frequency);
  u8g2.print("  Hz");
  u8g2.sendBuffer();
  // pwmWrite(led, brightness);   // выставляем скважность на пине
  // SetPinFrequency(led, frequency); // выставляем частоту на пине
  r = 1;
  e = 5;
}
////////////////////////// функция вольтметр
void voltmetr()
{               // функция вольтметр
  long sum = 0; // создаем пременную для вольтметра
  for (int i = 0; i < 50; i++)
    sum += analogRead(ADC_pin); // высчитывем среднее за 50 измерений
  Htime = (sum / 50);
  if (Htime >= Ltime + 2 || Htime <= Ltime - 2)
  {
    Ltime = Htime;
  }
  Ttime = ((40 + t) / 1024.0) * Ltime; // коррекция и приводим из ацп в вольты
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_10x20_mr);
  u8g2.setCursor(0, 13);
  u8g2.print("V");
  if (digitalRead(GEN0_pin) == 1)
  { // если на 10 пине высокий уровень то пишем N (нагрузка)
    u8g2.setCursor(0, 32);
    u8g2.print("N");
  }
  digitalWrite(LED_pin, 0); //????????
  if (digitalRead(GEN1_pin) == 1)
  { // если на 9 пине высокий уровень то пишем +
    u8g2.setCursor(0, 32);
    u8g2.print("+");
    if (Htime <= 5)
    {                           // если напряжение просело то пищим
      tone(s, 700, 150);        // подключаем пищалку на прозвонке
      digitalWrite(LED_pin, 1); // пождключаем светодиод на плате ардуино на прозвонке
    }
  }
  if (Htime >= 200 && Htime <= 250)
    digitalWrite(RX_LED_pin, LOW); // 2 строки светим светодиодом rx  если на игле около 12 вольт
  else
    digitalWrite(RX_LED_pin, HIGH);
  if (Htime >= 70 && Htime <= 100)
    digitalWrite(TX_LED_pin, LOW); // 2 строки светим светодиодом  tx если на игле около 5 вольт
  else
    digitalWrite(TX_LED_pin, HIGH);
  u8g2.setFont(u8g2_font_fub30_t_symbol);
  u8g2.setCursor(15, 32);
  u8g2.print(Ttime);
  u8g2.sendBuffer();
  if (Ltime <= 3)
    Ltime = 0;
  r = 1;
  e = 2;
}
////////////////////////// кан лин тестер
void can_lin_test()
{
  u8g2.firstPage();
  u8g2.setCursor(0, 13);
  u8g2.print("CAN LIN TEST");
  u8g2.setCursor(0, 32);
  long sum = 0;
  for (int i = 0; i < 500; i++)
  {
    sum = analogRead(ADC_pin);
    if (sum >= 25 && sum <= 35)
    { // настраиваем ацп для 1.3 вольта до 1.8 вольта примерно
      u8g2.print("  < CAN L >     ");
      u8g2.sendBuffer();
    }
    if (sum >= 45 && sum <= 70)
    { // настраиваем ацп для 3 вольта до 3.5 вольта примерно
      u8g2.print("  < CAN H >      ");
      u8g2.sendBuffer();
    }
    if (sum >= 150 && sum <= 600)
    { // настраиваем ацп для 7 вольта до 30 вольта примерно
      u8g2.print("  < LIN >     ");
      u8g2.sendBuffer();
    }
  }
  if (sum >= 0 && sum <= 10)
  { // настраиваем ацп около 0 вольт
    u8g2.print("  < TEST >      ");
    u8g2.sendBuffer();
  }
  r = 1;
  e = 6;
}
/////////////////////////функция таймер
void timers()
{
  if (millis() - timer > 1000)
  {
    timer = millis();
    u8g2.firstPage();
    u8g2.setCursor(0, 13);
    u8g2.print(g);
    u8g2.print(" IMP ");
    u8g2.print(n);
    u8g2.print(" WAIT");
    if (impyls == 0)
    {
      digitalWrite(led, LOW); // выключаем пин на время
      jdem = n + 1;
      impyls = g;
    }
    if (jdem == 1 && impyls >= 1)
    {
      u8g2.setCursor(0, 32);

      if (led == 9)
        u8g2.print("+ IMPYLS ");
      else
        u8g2.print("- IMPYLS ");
      u8g2.print(impyls);
      impyls--;
      tone(s, 500, 400);
      digitalWrite(led, HIGH); // включаем  пин на время
      delay(200);
    }

    else
    {
      jdem--;
      tone(s, 900, 50);
      u8g2.setCursor(0, 32);
      if (led == 9)
        u8g2.print("+ WAIT ");
      else
        u8g2.print("- WAIT ");
      u8g2.print(jdem);
    }
  }
  u8g2.sendBuffer();
  r = 1;
  e = 7;
}
/////////////////////////////////// функция дпкв
void dpkv()
{
  u8g2.firstPage();
  u8g2.setCursor(0, 13);
  u8g2.print(i);
  u8g2.print(" PIP ");
  u8g2.print(a);
  u8g2.print(" POP");
  u8g2.setCursor(0, 32);
  u8g2.print("TUNE  ");
  u8g2.sendBuffer();
  if (millis() - timer > 5000)
  {
    d = 1;
    tone(s, 500, 200);
    if (led == 9)
      u8g2.print("PULS +");
    else
      u8g2.print("PULS -");
    u8g2.sendBuffer();
    for (int vrom = 0; vrom < 20; ++vrom)
    { // общий цикл 20 оборотов колевала
      for (byte pip = 0; pip < i; ++pip)
      { // цикл для зубьев шкива
        digitalWrite(led, 1);
        delay(5);
        digitalWrite(led, 0);
        delay(5);
      }
      for (byte pop = 0; pop < a; ++pop)
      {           // цикл дя пропусков на шкиве
        delay(5); // скорость вращения, если заменить на переменную то можно регулировать
        delay(5);
      }
    }
  }
  if (d == 1)
  {
    d = 0;
    timer = millis();
    tone(s, 900, 100);
  }
  r = 1;
  e = 8;
}
/////////////////////////// функция осцилограф
void oscilograf()
{
  uint32_t start = micros();
  for (uint8_t x = 0; x < 128; ++x)
  { // 128 размер буфера
    delay(h);
    buffer[x] = map(analogRead(ADC_pin), 0, w, u8g2.getDisplayHeight() - 1, 0); // а7 пин ацп
  }
  u8g2.firstPage();
  do
  {
    drawScreen(start);
  } while (u8g2.nextPage());
  r = 1;
  e = 3;
}
void drawScreen(uint32_t duration)
{ // функция для работы осцилографа
  for (int8_t v = 5; v >= 0; --v)
  {
    uint8_t tickY = u8g2.getDisplayHeight() - u8g2.getDisplayHeight() * v / 5;
    for (uint8_t x = 0; x < u8g2.getDisplayWidth(); x += 8)
      u8g2.drawPixel(x, tickY);
  }
  for (uint8_t x = 1; x < 128; ++x)
  { // 128 размер буфера
    u8g2.drawLine(x - 1, buffer[x - 1], x, buffer[x]);
  }
  u8g2.setCursor(0, 13);
  u8g2.print(m);
}

void correction()
{                                      // функция коррекции напряжения
  rtime = analogRead(ADC_pin);         // берем информацию с ацп пин 7
  Ttime = ((40 + t) / 1024.0) * rtime; // вычисляем для отображения на дислее напряжения
  Ltime = rtime;                       // приводим из числа с точкой в обычное
  u8g2.firstPage();
  u8g2.setCursor(0, 13);
  u8g2.print(Ttime);
  u8g2.print("V  ");
  u8g2.print(Ltime);
  u8g2.print("ADC");
  u8g2.setCursor(0, 32);
  u8g2.print(t);
  u8g2.print(" COR SAVE2");
  u8g2.sendBuffer();
  r = 1;
  e = 9;
}
