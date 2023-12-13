#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "TickTwo.h"

// WIFI settings
const char* ssid = "";
const char* password = "";
IPAddress local_IP(192, 168, 100, 69);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(1, 1, 1, 1);  

// PIN SETTINGS
SoftwareSerial sensorSerial(2, 4); //4 NOT USED

// Remote server settings
const char* serverName = "http://192.168.100.88/new";
const char* accessToken = "";
const char* deviceID = "1";

// Global vars
uint8_t serialRxBuf[255];
uint8_t rxBufIdx = 0;
int sum = 0;
int sumCounter = 0;
HTTPClient http; // HTTP CLIENT
WiFiClient client;

void senData();

// Timer
TickTwo timer(senData, 1000*60); // Every minute...

void senData() { //perdiocally sends post request
  Serial.println("Trying to send data...");
  if(WiFi.status()!= WL_CONNECTED){
    return; // exit if wifi is not connected
  }
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Accept", "*/*");
  http.addHeader("Accept-Encoding", "gzip, deflate, br");
  http.addHeader("token", accessToken);
  if(sumCounter == 0){ //if there isn't new data exit the fuction
    Serial.println("No data avaible... skipping");
    return;
  }
  float temp = sum/sumCounter;
  String httpRequestData = "deviceID=" + String(deviceID) + "&PM25=" + String(temp);
  sum = 0;
  sumCounter = 0;
  int httpResponseCode = http.POST(httpRequestData); 
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end(); // Free resources
}


void setup() {
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  sensorSerial.begin(9600);
  Serial.begin(9600);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  timer.start();
}

void loop() {
  timer.update();
  if (sensorSerial.available()) {
        while (sensorSerial.available()) {
            
            int temp = sensorSerial.read();
            serialRxBuf[rxBufIdx++] = temp;
            Serial.print(temp, HEX);
            Serial.print(" ");

            delay(15); //for some reasons this delay is needed

            if (rxBufIdx >= 64) {
                memset(serialRxBuf, 0, sizeof(serialRxBuf));
                rxBufIdx = 0;
            }
        }
        
        rxBufIdx = 0;
        bool headerValid = serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B;
            if(headerValid){
              
              int pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];
              Serial.print(" decoded: ");
              Serial.println(pm25);
              memset(serialRxBuf, 0, sizeof(serialRxBuf));
              rxBufIdx = 0;
              sum = sum +  pm25;
              sumCounter++;
        }
        
   }
}
