#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup()   
{                
  Serial.begin(9600);
  attachInterrupt(1,start,RISING);     //аппаратное прерывание при прохождении первого датчика
  attachInterrupt(0,finish,RISING);      //аппаратное прерывание при прохождении второго датчика
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  updateScreen();
}

class button {
  public:
    button (byte pin) {
      _pin = pin;
      pinMode(_pin, INPUT_PULLUP);
    }
    bool click() {
      bool btnState = digitalRead(_pin);
      if (!btnState && !_flag && millis() - _tmr >= 100) {
        _flag = true;
        _tmr = millis();
        return true;
      }
      if (!btnState && _flag && millis() - _tmr >= 400) {
        _tmr = millis ();
        return true;
      }
      if (btnState && _flag) {
        _flag = false;
        _tmr = millis();
      }
      return false;
    }
  private:
    byte _pin;
    uint32_t _tmr;
    bool _flag;
    bool _long_flag;
};

bool flag = false;
bool long_flag = false;
uint32_t btnTimer = 0;
float speedArr[5];
float rapidArr[5];
float lastShot = 0;
float mass = 0.20;
float dist = 0.10;       //расстояние между датчиками в метрах  = 0.1
int rapidtime, show, counter, shots = 0;
volatile unsigned long gap1, gap2; // timestamp датчиков
unsigned long lastshotTime = 0;

button btn1(5); // screen
button btn2(4); // -
button btn3(7); // +
button btn4(6); // reset

void loop() {
//    Serial.print("sensor 1: ");
//    Serial.println(analogRead(2));  //показать значение на первом датчике
//    Serial.print("sensor 2: "); 
//    Serial.print(analogRead(3));   //показать значение на втором датчике
//    Serial.println();
//    delay(100);
  
  // поменять экран
  if (btn1.click()) {
    switch (show) {
      case 0:
        show = 1;
        display.clearDisplay();
        display.setTextSize(4);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("SPEED");
        display.setCursor(0,32);
        display.println("STATS");
        display.display();
        delay(500);
        break;
      case 1:
        show = 2;
        display.clearDisplay();
        display.setTextSize(4);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("RAPID");
        display.setCursor(0,32);
        display.println("MODE");
        display.display();
        delay(500);
        break;
      case 2:
        show = 3;
        display.clearDisplay();
        display.setTextSize(4);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("SET");
        display.setCursor(0,32);
        display.println("MASS");
        display.display();
        delay(500);
        break;
      case 3:
        show = 0;
        display.clearDisplay();
        display.setTextSize(4);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("SPEED");
        display.setCursor(0,32);
        display.println("MODE");
        display.display();
        gap1 = 0;
        delay(500);
        break;
    }
    updateScreen();
  }

  // кнопки +- для настройки массы шара
  if (btn2.click()) {
    changeMass(false);
  }

  if (btn3.click()) {
    changeMass(true);
  }

  // обнулить статы
  if (btn4.click()) resetStats();
  
  // определение скорости
  if (gap1 != 0 && gap2 != 0 && gap2 > gap1 && show != 2) {
    lastShot=(1000000*(dist)/(gap2-gap1));
    speedArr[counter] = lastShot;
    gap1=0;
    gap2=0;
    addOne();
    updateScreen();
  }
  if (micros()-gap1>1000000 && gap1!=0 && show!=2) { // ошибка если между 1 и 2 датчиков больше 1с
    screenFail();
    gap1=0;
    gap2=0;
  }

  // определение скорострельности
  if (show==2) {
    if (gap1!=0) {
      rapidtime=60/((float)(gap1-lastshotTime)/1000000);
      lastshotTime=gap1;
      rapidArr[counter] = rapidtime;
      addOne();
      
      gap1=0;
      showRapid();
    }
  }
  
  delay(50);
}

// скорость шара
void start() 
{
  if (gap1==0) {   //если измерение еще не проводилось
    gap1=micros(); //получаем время работы ардуино с момента включения до момента пролетания первой пули
  }
}
void finish() 
{
  if (gap2==0) {  //если измерение еще не проводилось
    gap2=micros();  //получаем время работы ардуино с момента включения до момента пролетания второй пули
  }
}

// обновить экран
void updateScreen() {
  switch (show) {
    case 0:
      screenSpeed();
      break;
    case 1:
      screenStats();
      break;
    case 2:
      showRapid();
      break;
    case 3:
      screenMass();
      break;
  }
}

// экран скорострельности
void showRapid() {  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(104,0);
  display.println("SHOT");
  display.drawLine(104, 8, 127, 8, WHITE);
  display.setCursor(104,10);
  display.println("MIN");
  display.setCursor(104,48);
  display.println("AVG");
  display.setCursor(104,56);
  display.println("OF 5");
  
  display.setTextSize(4);
  display.setCursor(0,0);
  display.println(rapidtime, 1);
  display.setCursor(0,35);
  int avgRp = 0;
  for (int i = 0; i <= 4; i++) {
    avgRp += rapidArr[i];
  }
  float avgRap = float(avgRp) / float(shots);
  if (shots > 0) display.println(avgRap, 0);
  else display.println("-");
  
  display.display();
}

// экран ошибки
void screenFail() {
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("FAIL");
  display.display();
  delay(1000);
  updateScreen();
}

// отрисовать скорость крупно
void screenSpeed() {
  display.clearDisplay();
  
  display.setTextColor(WHITE);
  display.setTextSize(4);
  display.setCursor(0,0);
  display.println(lastShot, 1);
  
  display.setTextSize(1);
  display.setCursor(122,0);
  display.println("M");
  display.drawLine(122, 8, 126, 8, WHITE);
  display.setCursor(122,10);
  display.println("S");
  
  display.setTextSize(4);
  display.setCursor(0,35);
  float shotPower = (float(lastShot)*float(lastShot)*float(mass))/2000;
  display.println(shotPower);
  
  display.setTextSize(1);
  display.setCursor(115,56);
  display.println("KJ");
  display.display();
}

// отрисовать статы мелко
void screenStats() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("1");
  display.setCursor(0,24);
  display.println("2");
  display.setCursor(0,48);
  display.println("3");
  display.setCursor(64,0);
  display.println("4");
  display.setCursor(64,24);
  display.println("5");
  display.setCursor(52,48);
  display.println("avg");
  
  display.setTextSize(2);
  display.setCursor(10,0);
  display.println(speedArr[0], 0);
  display.setCursor(10,24);
  display.println(speedArr[1], 0);
  display.setCursor(10,48);
  display.println(speedArr[2], 0);
  display.setCursor(74,0);
  display.println(speedArr[3], 0);
  display.setCursor(74,24);
  display.println(speedArr[4], 0);
  
  display.drawLine(64, 44, 128, 44, WHITE);
  
  display.setCursor(74,48);
  int avgSp = 0;
  for (int i = 0; i <= 4; i++) {
    avgSp += speedArr[i];
  }
  float avgSpeed = float(avgSp) / float(shots);
  if (shots > 0) display.println(avgSpeed, 0);
  else display.println("-");
  
  display.display();
}

// отрисовать экран настройки массы шара
void screenMass() {
  display.setTextSize(4);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(mass);
  
  display.setCursor(100,0);
  display.setTextSize(1);
  display.println("MASS");
  
  display.setCursor(100,4);
  display.setTextSize(3);
  display.println('g');
  display.display();
}

// обнулить статы
void resetStats() {
  for (int i = 0; i <= 4; i++) {
    speedArr[i] = 0;
  }
  for (int i = 0; i <= 4; i++) {
    rapidArr[i] = 0;
  }
  counter = 0;
  shots = 0;
  lastShot = 0;
  lastshotTime = 0;
  rapidtime = 0;
  gap1 = 0;
  gap2 = 0;
  updateScreen();
}

// меняем настройки массы шара
bool changeMass(bool flag) {
  if (show == 3) {
    if (flag == true) {
      if (mass <= 0.50) {
        mass += 0.01;
      }
    }
    else {
      if (mass >= 0.10) {
        mass -= 0.01;
      }
    } 
    updateScreen();
  }
}
