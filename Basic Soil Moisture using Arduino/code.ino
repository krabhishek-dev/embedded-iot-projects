#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd_1(0);

int sensorPin = A0;
int moistureValue = 0;

void setup()
{
  lcd_1.begin(16,2);
  lcd_1.print("Soil Moisture");
  delay(2000);
}

void loop()
{
  moistureValue = analogRead(sensorPin);

  lcd_1.setCursor(0,1);
  lcd_1.print("Value: ");
  lcd_1.print(moistureValue);
  lcd_1.print("   ");

  delay(500);
}