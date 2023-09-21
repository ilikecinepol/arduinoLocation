#include <Servo.h> 
#include <SPI.h>
#include <math.h> 
#include "Ucglib.h"           
// Библиотека для экрана.

#define trigPin   5          // Пин для передачи сигнала на ультразвуковой датчик (Trig), пин 6
#define echoPin   4          // Пин для приема сигнала от ультразвукового датчика (Echo), пин 5
#define ServoPin  3          // Пин для сервопривода основания, пин 3
#define ServoLaser  6          // Пин для сервопривода лазера, пин 6
#define ledPin  2          // Пин для сервопривода лазера, пин 6

int Ymax = 128;               // Высота экрана в пикселях
int Xmax = 160;               // Ширина экрана в пикселях
int Xcent = Xmax / 2;         // Середина экрана по горизонтали
int base = 118;               // Высота базовой линии
int scanline = 105;           // Длина луча радара
double laserDist;                      //  расстояние от лазера до препятствия
int laserAngle;                      // угол поворота лазера
double distance;
const float y = 4;                      // расстояние между осями вращения лазера и радара
const int maxDist = 30;       // Ограничение дальности действия
bool trackingMode = false;
int angleError;             // поправка на ошибку определения датчика из-за диаграммы направленности    
bool obstacleDetected = false;
int startAngle;
int endAngle;
int goalAngle;

Servo baseServo; 
Servo laserServo; 
Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

void setup(void)
{
     
      ucg.begin(UCG_FONT_MODE_SOLID); // Инициализация экрана
      ucg.setRotate90();              // Поворот экрана в горизонтальное положение. Если изображение перевернуто, вы можете изменить setRotate90 или setRotate270.
      
      pinMode(trigPin, OUTPUT);       // Установка пина для Trig в режим вывода
      pinMode(echoPin, INPUT);        // Установка пина для Echo в режим ввода
      pinMode(ledPin, OUTPUT);
      pinMode(ServoLaser, OUTPUT);
      digitalWrite(ledPin, 0);
      Serial.begin(115200);             // Настройка скорости передачи через последовательный порт
      baseServo.attach(ServoPin);     // Инициализация сервопривода
      laserServo.attach(ServoLaser);     // Инициализация сервопривода
    
      // Экран приветствия
      ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
      ucg.setColor(0, 0, 100, 0);
      ucg.setColor(1, 0, 100, 0);
      ucg.setColor(2, 20, 20,20);
      ucg.setColor(3, 20, 20, 20);
      ucg.drawGradientBox(0, 0, 160, 128);
      ucg.setPrintDir(0);
      ucg.setColor(0, 5, 0);
      ucg.setPrintPos(27,42);
      ucg.setFont(ucg_font_logisoso18_tf);  
      ucg.print("Mini Radar");
      ucg.setColor(0, 255, 0);
      ucg.setPrintPos(25,40);
      ucg.print("Mini Radar");
      ucg.setFont(ucg_font_helvB08_tf);
      ucg.setColor(0, 255, 0);
      ucg.setPrintPos(40,100);
      ucg.print("Testing...");
      baseServo.write(90);
      laserServo.write(90);
      ucg.setColor(0, 255, 0);
    
      // Проверка работы сервопривода. Убедитесь, что базовая позиция и вращение не блокируются (или что кабель не перекручен).
      for(int x=0;x<180;x+=5)
          { baseServo.write(x);
            laserServo.write(x);
            delay(50);
           }
      ucg.print("OK!");
      delay(500);
      ucg.setColor(0, 0, 0, 0);
      ucg.setColor(1, 0, 0, 0);
      ucg.setColor(2, 0, 0,0);
      ucg.setColor(3, 0, 0, 0);
    
      // Очистка экрана
      //ucg.clearScreen();
      cls();
      
      ucg.setFont(ucg_font_orgv01_hr);
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
}

void cls()
{
  // Очистка экрана
  ucg.setColor(0, 0, 0, 0);
  
  for(int s=0;s<128;s+=8)
  for(int t=0;t<160;t+=16)
  {
    ucg.drawBox(t,s,16,8);
   // delay(1);
  }
}

int calculateDistance()
{ 
      long duration;
      // Отключаем Trig и ждем 2 микросекунды
      digitalWrite(trigPin, LOW); 
      delayMicroseconds(2);
      // Включаем Trig, ждем 10 микросекунд и выключаем его
      digitalWrite(trigPin, HIGH); 
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Считываем время распространения звука (в микросекундах)
      duration = pulseIn(echoPin, HIGH);
      // Преобразуем время в расстояние
      return duration*0.034/2;
}
int goalDist;
void shot(int angle, int duration){
  
  
  //Serial.println(duration);
  if ((duration > 3) and (duration < maxDist) and (endAngle == 0)){
    
    if (obstacleDetected == false){
      startAngle = angle;
      Serial.print('startAngle: ');
      Serial.println(startAngle);
    }
    obstacleDetected = true;
    goalDist = duration;
    Serial.println('1');
    Serial.println(goalDist);
  }
  if ((obstacleDetected == true) and (duration > goalDist) and (endAngle == 0)){
    obstacleDetected = false;
    endAngle = angle;
    Serial.println('2');
    if ((startAngle != 0) and (endAngle != 0)){
    goalAngle = ((startAngle + endAngle) / 2);
    laserServo.write(goalAngle);
    digitalWrite(ledPin, HIGH);
    Serial.println('4');
  }
  }
  
  if (obstacleDetected == false){
    startAngle = 0;
    endAngle = 0;
    goalDist = 0;
    //digitalWrite(ledPin, LOW);
    Serial.println('3');
  }
  

  //digitalWrite(ledPin, HIGH);
  
}
void fix_font() 
{
      ucg.setColor(0, 180, 0);
      ucg.setPrintPos(70,14);
      ucg.print("1.00");
      ucg.setPrintPos(70,52);
      ucg.print("0.50");
      ucg.setPrintPos(70,90);
      ucg.print("0.25");
}
void calculateLaser(int angle, bool clockwise)
{
  angleError = map(angle, 0, 180, 60, 0);
  if (clockwise == true){
    angleError = - angleError;
  }
  //laserServo.attach(ServoLaser);
  if ((angle > 0) && (distance < maxDist)){
    laserDist = sqrtf(pow(distance,2)+pow(y, 2)-2*distance*y*cos(radians(angle)));
    laserAngle = degrees(acos(((y*y+laserDist*laserDist-distance*distance)/(2*laserDist*y))));
    //  Отладочный вывод: угол и расстояние
    Serial.print("distance: ");
    Serial.println(distance);
    Serial.print("laserDist: ");
    Serial.println(laserDist);
    Serial.print("Angle: ");
    Serial.println(angle);
    laserAngle = 180 - laserAngle;
    Serial.print("laserAngle: ");
    Serial.println(laserAngle);

    
    if ((laserAngle != 0) && (trackingMode == false)){
      trackingMode = true;
      laserServo.attach(ServoLaser);
      laserServo.write(angle + angleError);
      // digitalWrite(ledPin, 1);
      
      // laserServo.detach();
    }
  
    // return laserAngle;
  }
  
}

void fix()
{

      ucg.setColor(0, 40, 0);
      // Рисуем круговую основу
      ucg.drawDisc(Xcent, base+1, 3, UCG_DRAW_ALL); 
      ucg.drawCircle(Xcent, base+1, 115, UCG_DRAW_UPPER_LEFT);
      ucg.drawCircle(Xcent, base+1, 115, UCG_DRAW_UPPER_RIGHT);
      ucg.drawCircle(Xcent, base+1, 78, UCG_DRAW_UPPER_LEFT);
      ucg.drawCircle(Xcent, base+1, 78, UCG_DRAW_UPPER_RIGHT);
      ucg.drawCircle(Xcent, base+1, 40, UCG_DRAW_UPPER_LEFT);
      ucg.drawCircle(Xcent, base+1, 40, UCG_DRAW_UPPER_RIGHT);
      ucg.drawLine(0, base+1, Xmax,base+1);
     
      ucg.setColor(0, 120, 0);
      // Рисуем шкалу
       for(int i= 40;i < 140; i+=2)
       {

        if (i % 10 == 0) 
          ucg.drawLine(105*cos(radians(i))+Xcent,base - 105*sin(radians(i)) , 113*cos(radians(i))+Xcent,base - 113*sin(radians(i)));
        else
        
         ucg.drawLine(110*cos(radians(i))+Xcent,base - 110*sin(radians(i)) , 113*cos(radians(i))+Xcent,base - 113*sin(radians(i)));
       }
          
       // Рисуем некоторые декоративные элементы 
       ucg.setColor(0,200,0);
       ucg.drawLine(0,0,0,18);
       for(int i= 0;i < 5; i++)
       {
          ucg.setColor(0,random(200)+50,0);
          ucg.drawBox(2,i*4,random(14)+2,3);
       }

       ucg.setColor(0,180,0);
       ucg.drawFrame(146,0,14,14);
       ucg.setColor(0,60,0);
       ucg.drawHLine(148,0,10);
       ucg.drawVLine(146,2,10);
       ucg.drawHLine(148,13,10);
       ucg.drawVLine(159,2,10);
        
       ucg.setColor(0,220,0);
       ucg.drawBox(148,2,4,4);
       ucg.drawBox(148,8,4,4);
       ucg.drawBox(154,8,4,4);
       ucg.setColor(0,100,0);
       ucg.drawBox(154,2,4,4);

       ucg.setColor(0,90,0);
       ucg.drawTetragon(62,123,58,127,98,127,102,123);
       ucg.setColor(0,160,0);
       ucg.drawTetragon(67,123,63,127,93,127,97,123);
       ucg.setColor(0,210,0);
       ucg.drawTetragon(72,123,68,127,88,127,92,123);
}

void tracking()
{
  if (trackingMode == false)
  {
    laserServo.detach();
    //digitalWrite(ledPin, LOW);
  }
  else{
    laserServo.attach(ServoLaser);
    //digitalWrite(ledPin, LOW);
  }
}
void loop(void)
{
  
  // int distance;
  
  fix(); 
  fix_font();  // Обновляем фон экрана
  laserServo.attach(ServoLaser);
  for (int x=180; x > 4; x-=2){       // Сервопривод основания двигается от 180 до 0 градусов
     
      baseServo.write(x);             // Устанавливаем угол сервопривод
      
      // Рисуем линию сканирования радара
      int f = x - 4; 
      ucg.setColor(0, 255, 0);
      ucg.drawLine(Xcent, base, scanline*cos(radians(f))+Xcent,base - scanline*sin(radians(f)));
      f+=2;
      ucg.setColor(0, 128, 0);
      ucg.drawLine(Xcent, base, scanline*cos(radians(f))+Xcent,base - scanline*sin(radians(f)));
      f+=2;
      ucg.setColor(0, 0, 0);
      ucg.drawLine(Xcent, base, scanline*cos(radians(f))+Xcent,base - scanline*sin(radians(f)));
      ucg.setColor(0,200, 0);
      // Измеряем расстояние
      distance = calculateDistance();
      shot(x, distance);
      // Рисуем точку в соответствии с измеренным расстоянием
      if (distance < maxDist)
      {
        ucg.setColor(255,0,0);
        ucg.drawDisc(distance*cos(radians(x))+Xcent,-distance*sin(radians(x))+base, 1, UCG_DRAW_ALL);
      }
      else
      { // Если расстояние больше 1 метра, рисуем желтую точку в краевой области
        ucg.setColor(255,255,0);
        ucg.drawDisc(116*cos(radians(x))+Xcent,-116*sin(radians(x))+base, 1, UCG_DRAW_ALL);
      }
    
           
      // Отладочный вывод: угол и расстояние
      // Serial.print(x); 
      // Serial.print("    ,   ");
      // Serial.println(distance); 
     

      if (x > 70 and x < 110)  fix_font();  // Перерисовываем числа, когда линия сканирования пересекает числа
 
      ucg.setColor(0,0,155,  0);
      ucg.setPrintPos(0,126);
      ucg.print("DEG: "); 
      ucg.setPrintPos(24,126);
      ucg.print(x);
      ucg.print("  ");
      ucg.setPrintPos(125,126);
      ucg.print("  ");
      ucg.print(distance);
      ucg.print("cm  "); 
      
  }
  digitalWrite(ledPin, LOW);
  trackingMode = false;
  
  //ucg.clearScreen();  // Очищаем экран. Если уровень питания Arduino недостаточен, это может вызвать белый экран (прерывание сигнала отображения). Вы можете использовать cls(); вместо ucg.clearScreen(); 
  delay(50);
  cls();   // Если у вас часто возникают проблемы с белым экраном, вы можете использовать эту функцию, или увеличить мощность питания.

  fix(); 
  fix_font();          // Обновляем фон экрана
  laserServo.attach(ServoLaser);
  for (int  x=1; x < 176; x+=2){     
      baseServo.write(x);             // Устанавливаем угол сервопривода

      
      // Рисуем линию сканирования радара
      int f = x + 4;
      ucg.setColor(0, 255, 0);
      ucg.drawLine(Xcent, base, scanline*cos(radians(f))+Xcent,base - scanline*sin(radians(f)));
      f-=2;
      ucg.setColor(0, 128, 0);
      ucg.drawLine(Xcent, base, scanline*cos(radians(f))+Xcent,base - scanline*sin(radians(f)));
      f-=2;
      ucg.setColor(0, 0, 0);
      ucg.drawLine(Xcent, base, scanline*cos(radians(f))+Xcent,base - scanline*sin(radians(f)));
      ucg.setColor(0, 200, 0);
      // Измеряем расстояние

      distance = calculateDistance();
      shot(x, distance);
      // Рисуем точку в соответствии с измеренным расстоянием
      if (distance < maxDist)
      {
        ucg.setColor(255,0,0);
        ucg.drawDisc(distance*cos(radians(x))+Xcent,-distance*sin(radians(x))+base, 1, UCG_DRAW_ALL);
      }
      else
      { // Если расстояние больше 1 метра, рисуем желтую точку в краевой области
        ucg.setColor(255,255,0);
        ucg.drawDisc(116*cos(radians(x))+Xcent,-116*sin(radians(x))+base, 1, UCG_DRAW_ALL);
      }
           
      // Отладочный вывод: угол и расстояние  
      // Serial.print(x); 
      // Serial.print("    ,   ");
      // Serial.println(distance); 
     
      if (x > 70 and x < 110)  fix_font();  // Перерисовываем числа, когда линия сканирования пересекает числа
      
      ucg.setColor(0,0,155,  0);
      ucg.setPrintPos(0,126);
      ucg.print("DEG: "); 
      ucg.setPrintPos(24,126);
      ucg.print(x);
      ucg.print("   ");
      ucg.setPrintPos(125,126);
      ucg.print("   ");
      ucg.print(distance);
      ucg.print("cm   "); 
  
  }
  digitalWrite(ledPin, LOW);
  trackingMode = false;
 //ucg.clearScreen(); //
 delay(50);
 cls();
}
