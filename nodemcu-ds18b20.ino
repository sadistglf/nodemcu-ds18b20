#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>


#define ledPin 2
#define ONE_WIRE_BUS D3

char* ssid = "ssid";
char* password = "password";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);
  Serial.println();

  int connected = connectToWifi(ssid2, password2);
  if (connected == -1) {
    Serial.println("No se pudo conectar a red Wifi");
  }
  else {
    digitalWrite(ledPin, HIGH);
    Serial.println("Wifi Connected Succes!");
    Serial.print("NodeMCU IP Address: ");
    Serial.println(WiFi.localIP());
  }
  DS18B20.begin();
}

int connectToWifi(char* ssid, char* password) {
  int count = 0;
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");
  while ( WiFi.status() != WL_CONNECTED && count < 20) {
    delay(500);
    Serial.print(".");
    count += 1;
  }
  if (count == 20) {
    return -1;
  }
  else {
    return count;
  }
}

float blinker(int times, int tdelay) {
  int i;
  for (i = 0; i < times; i++) {
    digitalWrite(ledPin, LOW);
    delay(tdelay);
    digitalWrite(ledPin, HIGH);
    delay(tdelay);
  }
  return 1;
}
float getTemperature() {
  float temp;
  DS18B20.requestTemperatures();
  temp = DS18B20.getTempCByIndex(0);
  return temp;
}
void loop() {
  float currTemp;
  if (WiFi.status() == WL_CONNECTED) {
    // first we try to get the temperature
    currTemp = getTemperature();
    Serial.println(currTemp);


    String buf;
    buf += F("medido=2018-12-11T00:00:00");  // Here we should get the real timestamp. 
    buf += F("&valor=");
    buf += String(currTemp, 6);

    // Http connection!
    HTTPClient http;
    http.begin("http://www.pastebin.com/"); // PROVIDE REAL API URL
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(buf);
    blinker(5, 100); // just to do some visual checking!
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
    //ESP.deepSleep(30e6);   // TODO
    delay(10000);
  }
  else {
    Serial.println("Error in WiFi connection");
  }

}
