#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Servo.h>

#define Password_Lenght 5

// LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Servo
Servo gateServo;
#define SERVO_PIN A5

// Gas + Alarm
#define GAS_SENSOR A0
#define BUZZER 0   // D0

// Gate angles
#define OPEN_ANGLE 90
#define CLOSE_ANGLE 0

// Timing
unsigned long unlockTime = 0;
bool gateOpen = false;

unsigned long gasStartTime;
bool gasEnabled = false;

// Keypad
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {6, 7, 8, 9}; 
byte colPins[COLS] = {A1, A2, A3, A4}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Password variables
char Data[Password_Lenght]; 
char Data2[Password_Lenght];
char Master[Password_Lenght];

byte data_count = 0;
byte mode = 0;

char key;
byte key_state = 0;
char last_press_key;

// Long press
unsigned long keyPressTime = 0;

// Default password
char FistTimePassword[] = {'2','5','8','0'};

void setup(){
  Serial.begin(9600);
  Check_EEPROM();

  lcd.begin(16, 2);

  pinMode(13, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  gateServo.attach(SERVO_PIN);
  gateServo.write(CLOSE_ANGLE);

  gasStartTime = millis();
}

void loop()
{
  key = keypad.getKey(); 
  key_state = keypad.getState();

  // Enable gas sensor after 30 sec
  if(millis() - gasStartTime > 30000){
    gasEnabled = true;
  }

  // GAS SENSOR LOGIC
  if(gasEnabled){
    int gasValue = analogRead(GAS_SENSOR);
    if(gasValue > 400){  // threshold (adjust if needed)
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("FIRE ALERT!");

      digitalWrite(BUZZER, HIGH);
      delay(2000);

      lcd.clear();
      lcd.print("Sir Go Away");
      delay(2000);

      lcd.clear();
      lcd.print("Emergency Open");

      gateServo.write(OPEN_ANGLE);
      gateOpen = true;

      delay(5000);
      digitalWrite(BUZZER, LOW);
    }
  }

  if(key){
    last_press_key = key;
    Serial.println(key);

  }


  // AUTO LOCK AFTER 10 SEC
  if(gateOpen && (millis() - unlockTime > 10000)){
    gateServo.write(CLOSE_ANGLE);
    lcd.clear();
    lcd.print("Door Locking...");
    delay(2000);
    gateOpen = false;
    mode = 0;
  }

  // EXISTING LOGIC
  if(mode == 3){
    if(last_press_key == '#' && key_state == 2){
      mode = 1;
    }
    if(last_press_key == '*' && key_state == 2){
      mode = 0;
      lcd.clear();
      lcd.print("LOCKED");
      digitalWrite(13, LOW); 
      delay(2000);
    }        
  }

  if(mode == 0){
    lcd.setCursor(1,0);
    lcd.print("Enter Password");
  }
  else if(mode == 1){
    lcd.print("Set New Password");
  }
  else if(mode == 2){
    lcd.print("Password Again");
  }
  else if(mode == 3){
    lcd.setCursor(4,0);
    lcd.print("UNLOCKED");
  }

  if (key && key != '#' && mode != 3) {
    collectKey();
  }

  if(data_count == Password_Lenght-1){
    if(mode == 0){
      lcd.clear();
      if(!strcmp(Data, Master)) {

        lcd.setCursor(0,0);
        lcd.print("Welcome Back");
        lcd.setCursor(0,1);
        lcd.print("Abhishek");

        delay(2000);

        lcd.clear();
        lcd.print("Gate Opening...");

        gateServo.write(OPEN_ANGLE);
        gateOpen = true;
        unlockTime = millis();

        digitalWrite(13, HIGH);

        delay(2000);
        mode = 3;

      } else {
        lcd.print("INCORRECT");
        delay(2000);
      }

      lcd.clear();
      clearData();
    }

    else if(mode == 1){
      mode = 2;
      for(int i = 0; i < Password_Lenght; i++){
        Data2[i] = Data[i];
      }
      clearData();
    }

    else if(mode == 2){
      if(!strcmp(Data, Data2)){
        EEPROM.put(0, Data);
        EEPROM.get(0, Master);
      }
      mode = 3;
      clearData();
      lcd.clear();
    }
  }
}

void collectKey(){
  Data[data_count] = key; 
  lcd.setCursor(4+data_count,1); 
  lcd.print("*"); 
  data_count++;   
}

void clearData(){
  while(data_count != 0){
    Data[data_count--] = 0; 
  }
}

void Check_EEPROM(){
  EEPROM.get(0, Master);
  if(Master[0] == 0){
    EEPROM.put(0, FistTimePassword);
    EEPROM.get(0, Master);
  }
}