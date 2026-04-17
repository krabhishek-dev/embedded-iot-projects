// Automatic Street Light using LDR

int ldrPin = A0;
int ledPin = 11;

int lightValue = 0;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  // Read light intensity
  lightValue = analogRead(ldrPin);

  Serial.print("Light Value: ");
  Serial.println(lightValue);

  // Dark condition
  if (lightValue < 400)
  {
    digitalWrite(ledPin, HIGH);   // Street light ON
  }
  else
  {
    digitalWrite(ledPin, LOW);    // Street light OFF
  }

  delay(500);
}