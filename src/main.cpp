#include <Arduino.h>

#include <WiFi.h>
//#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "time.h"

unsigned long epochTime; 
unsigned long dataMillis = 0;

const char* ntpServer = "pool.ntp.org";
const char* serverName = "https://data.mongodb-api.com/app/application-0-kjgdz/endpoint/iot";

StaticJsonDocument<500> doc;

void POSTData(void);
unsigned long getTime(void);

void setup() {

  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(0, 0, ntpServer);  
}

void loop() {

if (millis() - dataMillis > 15000 || dataMillis == 0)
   {
       dataMillis = millis();

       epochTime = getTime();
       Serial.print("Epoch Time: ");
       Serial.println(epochTime);

       float temperature = 28.4;//dht.readTemperature();
       float humidity = 45.2;//dht.readHumidity();

       //digitalWrite(SOILPOWER, HIGH);
       //delay(10);
       float moisture = 2048;//analogRead(SOILPIN);
       float moisturePercent = 100.00 - ( (moisture / 4095.00) * 100.00 );
       //digitalWrite(SOILPOWER, LOW);

       float ldr = 666;//analogRead(LDRPIN);
       float ldrPercent = (ldr / 4095.00) * 100.00;

       Serial.print("Temperature: ");
       Serial.print(String(temperature));
       Serial.print(" C\nHumidity: ");
       Serial.print(String(humidity));
       Serial.print("\nMoisture: ");
       Serial.print(String(moisturePercent));
       Serial.print(" %");
       Serial.print("\nLight: ");
       Serial.print(String(ldrPercent));
       Serial.print(" %");
       Serial.println("\n");

       doc["sensors"]["temperature"] = temperature;
       doc["sensors"]["humidity"] = humidity;
       doc["sensors"]["moisture"] = moisturePercent;
       doc["sensors"]["light"] = ldrPercent;
       doc["sensors"]["timestamp"] = epochTime;

       Serial.println("Uploading data... "); 
       POSTData();
   }
}

// Function that gets current epoch time
unsigned long getTime(void) {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return now;
}

void POSTData(void)
{
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      String json;
      serializeJson(doc, json);

      Serial.println(json);
      int httpResponseCode = http.POST(json);
      Serial.println(httpResponseCode);

      if (httpResponseCode == 200) {
        Serial.println("Data uploaded.");
        //digitalWrite(LEDGREENPIN, HIGH);
        //delay(200);
        //digitalWrite(LEDGREENPIN, LOW);
      } else {
        Serial.println("ERROR: Couldn't upload data.");
        //digitalWrite(LEDREDPIN, HIGH);
        //delay(200);
        //digitalWrite(LEDREDPIN, LOW);
      }

    }
}