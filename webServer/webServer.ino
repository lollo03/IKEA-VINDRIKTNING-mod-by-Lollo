#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WIFI settings
const char* ssid = ""; //Insert SSID
const char* password = ""; //Insert wifi password

ESP8266WebServer server(80);

SoftwareSerial sensorSerial(2, 4); //4 NOT USED

IPAddress local_IP(192, 168, 100, 69); // Delete these lines for DHCP
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(1, 1, 1, 1);  
// Stop deleting here

// Global vars
uint8_t serialRxBuf[255];
uint8_t rxBufIdx = 0;
uint8_t temp = 0;
uint16_t pm25 = 0;
uint16_t maxPm25 = 0;
uint16_t countReader = 0;

void setup() {
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) { // Delete these lines for DHCP
    Serial.println("STA Failed to configure");
  }
  // Stop deleting here


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

  server.on("/", []() {
      server.send(200, "text/plain", "Last read: " + String(pm25) + "\nmax pm2.5: " + String(maxPm25) + "\nReads N. " + String(countReader));
  });

  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {
  
  server.handleClient();
  if (sensorSerial.available()) {
        while (sensorSerial.available()) {
            
            temp = sensorSerial.read();
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
              
              pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];
              Serial.print(" dal sensore: ");
              Serial.println(pm25);
              memset(serialRxBuf, 0, sizeof(serialRxBuf));
              rxBufIdx = 0;
              countReader++;
              if(pm25 > maxPm25)
                maxPm25 = pm25;
        }
        Serial.print("\n");
        
   }
}
