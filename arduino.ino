#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <EEPROM.h>
LiquidCrystal_I2C lcd(0x3f,20,4);


// Энкодер
#define pin_CLK 6
#define pin_DT  5
#define pin_Btn 7
unsigned long CurrentTime, LastTime;
enum eEncoderState {eNone, eLeft, eRight, eButton};
uint8_t EncoderA, EncoderB, EncoderAPrev;
int16_t counter;
bool ButtonPrev;
eEncoderState GetEncoderState() {
  // Считываем состояние энкодера
  eEncoderState Result = eNone;
  CurrentTime = millis();
  if (CurrentTime - LastTime >= 20) {
    // Считываем не чаще 1 раза в 5 мс для уменьшения ложных срабатываний
    LastTime = CurrentTime;
      EncoderA = digitalRead(pin_DT);
       EncoderB = digitalRead(pin_CLK);
      if ((!EncoderA) && (EncoderAPrev)) { // Сигнал A изменился с 1 на 0
        if (EncoderB) Result = eRight;     // B=1 => энкодер вращается по часовой
        else          Result = eLeft;      // B=0 => энкодер вращается против часовой
      }
      EncoderAPrev = EncoderA; // запомним текущее состояние
    }
    
  
  return Result;
}

void EEPROM_float_write(int addr, float val) // запись в ЕЕПРОМ
{
  byte *x = (byte *)&val;
  for (byte i = 0; i < 5; i++) EEPROM.write(i + addr, x[i]);
}

float EEPROM_float_read(int addr) // чтение из ЕЕПРОМ
{
  byte x[5];
  for (byte i = 0; i < 5; i++) x[i] = EEPROM.read(i + addr);
  float *y = (float *)&x;
  return y[0];
}

// Энкодер
int vdu = 0;
int wash = 0;
int vduWait = 0;
int washWait = 0;
int vduEmergency = 0;
int emergencyBreak = 0;

// Датчики, кнопки, лампочки
int changeMode = 9;
int joystick = A3;
int stopButton = 3;
int startButton = 4;
int greenButton = 13;
int redButton = 2;
int dat1 = A0;
int R1 = 8; // вверх
int R2 = 12; // вниз
int R3 = 11; // влево
int R4 = 10; // вправо
int R5 = A2; //вду
int R6 = A1; // мойка
int knopka_a = 0;
const int green = 13;

// Меню для временных значених
int set = 0;
int x = 0;
int y = 0;
bool off = true;

//флаги
int flagTo580_620 = 1;
int flagTo490_530 = 1;
int flagTo390_430 = 1;
int mainKey = 0;

void setup() {
//загружаем настройки из памяти МК
  vdu = EEPROM_float_read(0);
  wash = EEPROM_float_read(4);
  vduWait = EEPROM_float_read(8);
  washWait = EEPROM_float_read(16);

  if (isnan(vdu)) { //если нет настоек по умолчанию
    //загружаем их в память
     EEPROM_float_write(0, 15);
     EEPROM_float_write(4, 4);
     EEPROM_float_write(8, 4);
  vdu = EEPROM_float_read(0);
  wash = EEPROM_float_read(4);
  vduWait = EEPROM_float_read(8);
  washWait = EEPROM_float_read(16);
  }
  
  pinMode(joystick, OUTPUT);
  pinMode(startButton, INPUT);
  pinMode(dat1, INPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  pinMode(R5, OUTPUT);
  pinMode(R6, OUTPUT);
  pinMode(greenButton, OUTPUT);
  pinMode(redButton, OUTPUT);
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("Project by");
  lcd.setCursor(3,1);
  lcd.print("Stremilov Lev");
  lcd.setCursor(0,3);
  lcd.print("Power By SVARTON");
  //Энкодер установка
  pinMode(pin_DT,  INPUT);
  pinMode(pin_CLK, INPUT);
  pinMode(pin_Btn, INPUT); // Кнопка не подтянута к +5 поэтому задействуем внутренний pull-up резистор
  Serial.begin(9600);
  //Подключение порта
  digitalWrite(pin_Btn, 1);
  digitalWrite(pin_CLK, 1);
  digitalWrite(pin_DT, 1);
  digitalWrite(startButton, 1);
  digitalWrite(stopButton, 1);
  digitalWrite(changeMode, 1);
  pinMode(knopka_a, INPUT);
  Serial.begin(9600);
  delay(2000);
lcd.clear();
}

void save() {
  EEPROM_float_write(0, vdu);
  EEPROM_float_write(4, wash);
  EEPROM_float_write(8, vduWait);
  EEPROM_float_write(16, washWait);
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("       MODE SAVE  ");
  lcd.setCursor (0, 2);
  lcd.print("         WAIT   ");
  delay(1500);
  set = 0; //выходим из меню
  lcd.clear();
}


void loop() {
  x = analogRead(dat1);
  lcd.setCursor(1,0);
  lcd.print(x);
  Serial.println(x);

if (digitalRead(stopButton) == 1 || emergencyBreak == 1){
  emergencyBreak = 0;
  mainKey = 0;
  digitalWrite(redButton, LOW);
  lcd.clear();
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(R3, LOW);
  digitalWrite(R4, LOW);
  digitalWrite(R5, LOW);
  digitalWrite(R6, LOW);
  lcd.setCursor(0,1);
  lcd.print("PRESSED STOP");
  lcd.setCursor(12,2);
  lcd.print("BUTTON");
  delay(1000);
  flagTo580_620 = 1;
  flagTo490_530 = 0;
  flagTo390_430 = 0;
  set = 0;
  lcd.clear();
} 

if (digitalRead(changeMode) == 1){
  digitalWrite(joystick, 1);
} else digitalWrite(joystick, 0);
  
if (digitalRead(pin_Btn) == 0 && knopka_a == 0) { // нажата ли кнопка (menu)
      knopka_a = 1;
      set = set + 1; //перебираем меню
      lcd.clear();
if (set > 4) { //Задействован один из режимов защиты, а этой кнопкой мы его вырубаем. (или мы просто дошли до конца меню)
        off = false;
        save();
        set = 0;//выходим из меню
}
}
if (digitalRead(pin_Btn) == 1 && knopka_a == 1) knopka_a = 0;

if (set == 0 && digitalRead(changeMode) == 0){
  digitalWrite(greenButton, HIGH);
  lcd.setCursor(14,3);
  lcd.print("    ");
  lcd.setCursor(1,2);
  lcd.print("WAITING FOR START");
  lcd.setCursor(1,4);
if (digitalRead(startButton) == 0 && digitalRead(stopButton) == 0 || mainKey == 1) {
  mainKey = 1;
  digitalWrite(greenButton, LOW);
  digitalWrite(redButton, HIGH);

if (x >= 580 && x < 620 && flagTo580_620 == 1){  // едем к вду
  flagTo580_620 = 0;
  lcd.clear();
  digitalWrite(R4, HIGH);
  flagTo490_530 = 1;
}

if (x >= 580 && x < 620 && flagTo580_620 == 2){
  flagTo580_620 = 1;
  lcd.clear();
  digitalWrite(R3, LOW);
  delay(1000);
  digitalWrite(R6, HIGH);
  delay(1000);
  digitalWrite(R2, HIGH);
  int washingTime = 0;
  for (washingTime = 0; washingTime < wash; washingTime++){
    if (digitalRead(stopButton) == 1){
      emergencyBreak = 1;
      break;
    }
    lcd.setCursor(6, 2);
    lcd.print("WASHING");
    lcd.setCursor(6, 3);
    lcd.print("MOVING DOWN");
    delay(1000);
  }
  digitalWrite(R2, LOW);

  int washWaitTimer = 0;
  for (washWaitTimer = 0; washWaitTimer < washWait; washWaitTimer++){
    if (digitalRead(stopButton) == 1){
      emergencyBreak = 1;
      break;
    }
    lcd.setCursor(3, 2);
    lcd.print("WASHING");
    lcd.setCursor(4, 3);
    lcd.print("BOTTON POSITION");
    delay(1000);
  }

  digitalWrite(R1, HIGH);
  lcd.clear();
  washingTime = 0;
  for (washingTime = 0; washingTime < wash; washingTime++){
    if (digitalRead(stopButton) == 1){
      emergencyBreak = 1;
      break;
    }
    lcd.setCursor(6, 2);
    lcd.print("WASHING");
    lcd.setCursor(6, 3);
    lcd.print("MOVING UP");
    delay(1000);
  }
  lcd.clear();
  digitalWrite(R1, LOW);
  digitalWrite(R6, LOW);
  lcd.clear();
  digitalWrite(R3, LOW);
  digitalWrite(redButton, LOW);
  mainKey = 0;
}

if (x >= 490 && x < 520 && flagTo490_530 == 1){ // работает вду и начало движения в сторону наколнной плоскости
  flagTo580_620 = 2;
  flagTo490_530 = 0;
  lcd.clear(); 
  digitalWrite(R4, LOW);
  delay(1000);
  digitalWrite(R5, HIGH);
  delay(1000);
  digitalWrite(R2, HIGH);
  int vduWashingTime = 0;
  for (vduWashingTime = 0; vduWashingTime < vdu; vduWashingTime++){
    if (digitalRead(stopButton) == 1){
      emergencyBreak = 1;
      break;
    }
    lcd.setCursor(4, 2); 
    lcd.print("VDU WORKING");
    delay(500);
  }
  lcd.clear();
  digitalWrite(R2, LOW);

  int vduWaitTimer = 0;
  for (vduWaitTimer = 0; vduWaitTimer < vduWait; vduWaitTimer++){
    if (digitalRead(stopButton) == 1){
      emergencyBreak = 1;
      break;
    }
    lcd.setCursor(3, 2);
    lcd.print("VDU WORKING");
    lcd.setCursor(4, 3);
    lcd.print("BOTTON POSITION");
    delay(1000);
  }
  digitalWrite(R5, LOW);
  if (digitalRead(R5) == 1){  
    emergencyBreak = 1;
  }
  digitalWrite(R1, HIGH);
  lcd.clear();
  vduWashingTime = 0;

  for (vduWashingTime = 0; vduWashingTime < vdu; vduWashingTime++){
    if (digitalRead(stopButton) == 1){
      emergencyBreak = 1;
      break;
    }
    lcd.setCursor(4, 2);
    lcd.print("VDU WORKING");
    delay(1000);
  }
  lcd.clear();
  digitalWrite(R1, LOW);
  digitalWrite(R4, HIGH);
  lcd.setCursor(8, 2);
  lcd.print("-");
  lcd.setCursor(9, 2);
  lcd.print("-");
  lcd.setCursor(10, 2);
  lcd.print("-");
  lcd.setCursor(11, 2);
  lcd.print(">");
  lcd.clear();
  flagTo390_430 = 1;
}

if (x >= 390 && x <= 430 && flagTo390_430 == 1){
  flagTo390_430 = 0;
  flagTo580_620 = 2;
  lcd.clear(); // лежит на наклонной плоскости, затем двигается к мойке
  digitalWrite(R4, LOW);
  lcd.setCursor(6, 2);
  lcd.print("CLEARING");
  delay(5000);
  lcd.clear();
  digitalWrite(R3, HIGH);
  lcd.setCursor(8, 2);
  lcd.print("<");
  lcd.setCursor(9, 2);
  lcd.print("-");
  lcd.setCursor(10, 2);
  lcd.print("-");
  lcd.setCursor(11, 2);
  lcd.print("-");
}
}
}

if (set == 1){
  lcd.setCursor(0,3);
  lcd.print("VDU UP/DOWN time:");
  lcd.setCursor(17,3);
  lcd.print(vdu);
  digitalWrite(greenButton, LOW);
  if ( vdu < 10 ){
  lcd.setCursor(18,3);
  lcd.print("  ");
  }
}

if (set == 2){
  digitalWrite(greenButton, LOW);
  lcd.setCursor(0,3);
  lcd.print("Wash UP/DOWN time:");
  lcd.setCursor(18,3);
  lcd.print(wash);
  if ( wash < 10 ){
  lcd.setCursor(19,3);
  lcd.print(" ");
  }
}

if (set == 3){
  digitalWrite(greenButton, LOW);
  lcd.setCursor(0,3);
  lcd.print("VDU work time:");
  lcd.setCursor(15,3);
  lcd.print(vduWait);
  if ( vduWait < 10 ){
    lcd.setCursor(16,3);
  lcd.print(" ");
  }
}

if (set == 4){
  digitalWrite(greenButton, LOW);
  lcd.setCursor(0,3);
  lcd.print("Wash work time:");
  lcd.setCursor(16,3);
  lcd.print(washWait);
  if ( washWait < 10 ){
    lcd.setCursor(17,3);
  lcd.print(" ");
  }
}

// настройки вращения энкодера VDU
if (set == 1){ 
switch (GetEncoderState()) {
    case eNone: return;
    case eLeft: {   // Энкодер вращается влево  
       vdu--;   
        break;  
      }
    case eRight: {  // Энкодер вращается вправо
        vdu++;      
        break;
      }  
      }
}

// настройки вращения энкодера WASH
if (set == 2){
switch (GetEncoderState()) {
    case eNone: return;
    case eLeft: {   // Энкодер вращается влево
        wash--;
        break;
      }
    case eRight: {  // Энкодер вращается вправо 
        wash++;      
        break;
      }  
      }
}

if (set == 3){
switch (GetEncoderState()) {
    case eNone: return;
    case eLeft: {   // Энкодер вращается влево
        vduWait--;
        break;
      }
    case eRight: {  // Энкодер вращается вправо 
        vduWait++;      
        break;
      }  
      }
}

if (set == 4){
switch (GetEncoderState()) {
    case eNone: return;
    case eLeft: {   // Энкодер вращается влево
        washWait--;
        break;
      }
    case eRight: {  // Энкодер вращается вправо 
        washWait++;      
        break;
      }  
      }
}



if ( wash < 0 ){
  wash = 0;
}
if ( vdu < 0 ){
  vdu = 0;
}
if ( vduWait < 0 ){
  vduWait = 0;
}

if (set == 2){
  if ( wash > 60 ){
    int washMin = 1; 
    lcd.setCursor(14,3);
    lcd.print(washMin);
  }
}
}
