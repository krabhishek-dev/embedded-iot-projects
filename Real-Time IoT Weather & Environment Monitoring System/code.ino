#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <SparkFun_SCD4x_Arduino_Library.h>

// ---------------- PINS ----------------
#define DHTPIN D3   
#define DHTTYPE DHT11

#define LDR_PIN D5        // DIGITAL now
#define MQ135_PIN A0      // ANALOG gas sensor

// ---------------- WIFI ----------------
const char* ssid     = "your_ssid";
const char* password = "ssid_password";
const char* mqtt_server = "your_IPv4";

// ---------------- OBJECTS ----------------
DHT dht(DHTPIN, DHTTYPE);
SCD4x scd4x;

WiFiClient espClient;
PubSubClient client(espClient);

// ---------------- VARIABLES ----------------
long lastMsg = 0;
float temp, humi;
int ldrState;
int gasValue;

uint16_t co2;
float co2_temp, co2_humi;

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(LDR_PIN, INPUT);

  Wire.begin(D2, D1);  

  if (!scd4x.begin()) {
    Serial.println("SCD4x not detected!");
    while (1);
  }

  scd4x.startPeriodicMeasurement();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

// ---------------- WIFI ----------------
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

// ---------------- MQTT ----------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266ClientNodeMCU")) {
      Serial.println("connected");
    } else {
      Serial.print("failed rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");
      delay(5000);
    }
  }
}

// ---------------- LOOP ----------------
void loop() {

  if (!client.connected()) reconnect();
  client.loop();

  // DHT
  humi = dht.readHumidity();
  temp = dht.readTemperature();

  // LDR DIGITAL (0 or 1)
  ldrState = digitalRead(LDR_PIN);

  // MQ135 ANALOG
  gasValue = analogRead(MQ135_PIN);

  // CO2 Sensor
  if (scd4x.readMeasurement()) {
    co2 = scd4x.getCO2();
    co2_temp = scd4x.getTemperature();
    co2_humi = scd4x.getHumidity();
  }

  if (isnan(temp) || isnan(humi)) {
    Serial.println("DHT Error!");
    return;
  }

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    Serial.println("------ DATA ------");
    Serial.println("Temp: " + String(temp));
    Serial.println("Humidity: " + String(humi));
    Serial.println("LDR (Digital): " + String(ldrState));
    Serial.println("Gas (MQ135): " + String(gasValue));
    Serial.println("CO2: " + String(co2));

    // MQTT publish
    client.publish("tempNode", String(temp).c_str());
    client.publish("humiNode", String(humi).c_str());
    client.publish("ldrNode", String(ldrState).c_str());
    client.publish("gasNode", String(gasValue).c_str());
    client.publish("co2Node", String(co2).c_str());
  }

  delay(2000);
}