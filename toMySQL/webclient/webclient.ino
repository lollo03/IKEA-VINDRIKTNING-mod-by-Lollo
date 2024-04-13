#define LED_BUILTIN 2

#include <SoftwareSerial.h>
#include <Wire.h> 

// https://github.com/enjoyneering/AHTxx
#include <AHTxx.h>

AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR); 


#include "ScioSense_ENS160.h"  
//ScioSense_ENS160 ens160(ENS160_I2CADDR_0);
ScioSense_ENS160 ens160(ENS160_I2CADDR_1);

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//Defines
//#define static_ip  //Uncomment to enable static ip
//#define debug_wifi  //Uncomment to enable wifi debug to serial console

#define AHT_ENS_SDA D6
#define AHT_ENS_SCL D7
#define PM25_SDA D1
#define PM25_SCL D2 //set to an empty pin

// PM25 serial
SoftwareSerial sensorSerial(PM25_SDA, PM25_SCL);

//Global vars
float temperature;
float humidity;
float oldtemp;
float oldhum;
int tvoc;
int eCO2;
HTTPClient http; // HTTP CLIENT
WiFiClient client;
int readsCounter = 0;
float tempSum = 0;
float humiditySum =0;
float tvocSum = 0;
float eCO2Sum = 0;
uint8_t serialRxBuf[255];
uint8_t rxBufIdx = 0;
uint16_t pm25 = 0;


// WIFI settings
const char* ssid = ""; // SSID 
const char* password = ""; // WIFI PASSWORD
# if defined(static_ip)
IPAddress local_IP(192, 168, 100, 70); 
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(1, 1, 1, 1);  
# endif


// Remote server settings
const char* serverName = "";
const char* accessToken = ""; //max 64 chars
const char* deviceID = "";

void senData() { //periodically sends post request
  Serial.println("Trying to send data...");
  if(WiFi.status()!= WL_CONNECTED){
    return; // exit if wifi is not connected
  }
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Accept", "*/*");
  http.addHeader("Accept-Encoding", "gzip, deflate, br");
  http.addHeader("token", accessToken);

  String httpRequestData = "deviceID=" + String(deviceID) + "&eCO2=" + String(eCO2Sum/readsCounter) + "&TVOC=" + String(tvocSum/readsCounter) + "&temperature=" + String(tempSum/readsCounter) + "&humidity=" +String(humiditySum/readsCounter) + "&PM25=" + String(pm25);

  readsCounter = 0;
  tempSum = 0;
  humiditySum =0;
  tvocSum = 0;
  eCO2Sum = 0;

  int httpResponseCode = http.POST(httpRequestData); 
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end(); // Free resources
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //Configures static IP address
  # if defined(static_ip)
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) { //Delete here for DHCP
    Serial.println("STA Failed to configure");
  }
  # endif

  //Debug for wifi
  # if defined(debug_wifi)
  Serial.setDebugOutput(true);
  # endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ens160.setI2C(AHT_ENS_SDA, AHT_ENS_SCL);
  if(!ens160.begin()){
    Serial.println("Errore ens160");
  }
  ens160.setMode(ENS160_OPMODE_STD);

  if(!aht20.begin(AHT_ENS_SDA, AHT_ENS_SCL)){
    Serial.println("Errore aht20");
  }
  
  sensorSerial.begin(9600);

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  if(aht20.getStatus() == AHTXX_NO_ERROR){
    temperature = aht20.readTemperature();
    humidity = aht20.readHumidity();
  }

  if(temperature != 255.0 || humidity != 255.0){ //if the values are strange use the last know value
    oldtemp = temperature;
    oldhum = humidity;
  }else{
    temperature = oldtemp;
    humidity = oldhum;
  }

  Serial.println();
  Serial.print("Temperature: " + String(temperature));
  Serial.println(" Humidity: " + String(humidity));

  if (ens160.available()) {
    ens160.set_envdata(temperature,humidity);
    ens160.measure(true);
    ens160.measureRaw(true);
  }
  
  tvoc = ens160.getTVOC();
  eCO2 = ens160.geteCO2();
  Serial.print("TVOC: ");Serial.print(tvoc);Serial.print("ppb\t");
  Serial.print("eCO2: ");Serial.print(String(eCO2));Serial.println("ppm\t");
  
  Serial.println();

  if (sensorSerial.available()) {
        while (sensorSerial.available()) {
            serialRxBuf[rxBufIdx++] = sensorSerial.read();
            delay(15); //for some reasons this delay is needed
            if (rxBufIdx >= 64) {
                memset(serialRxBuf, 0, sizeof(serialRxBuf));
                rxBufIdx = 0;
            }
        }
        rxBufIdx = 0;
        bool headerValid = serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B;
            if(headerValid){
              pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];
              Serial.print("PM25: ");
              Serial.println(pm25);
              memset(serialRxBuf, 0, sizeof(serialRxBuf));
              rxBufIdx = 0;
        }
        Serial.print("\n");
        
   }


  tempSum += temperature;
  humiditySum += humidity;
  tvocSum += tvoc;
  eCO2Sum += eCO2;
  readsCounter++;

  if(readsCounter >= 4){
    senData();
  }
  delay(5000);
}