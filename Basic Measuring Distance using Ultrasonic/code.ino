#include <Adafruit_LiquidCrystal.h>

#define trigPin 2
#define echoPin 3

long duration;
int distance;

Adafruit_LiquidCrystal lcd_1(0);

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  lcd_1.begin(16, 2);
  lcd_1.print("Distance:");
}

void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  lcd_1.setCursor(0,1);
  lcd_1.print(distance);
  lcd_1.print(" cm   ");

  delay(500);
}