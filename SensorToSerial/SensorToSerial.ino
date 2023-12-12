#include <SoftwareSerial.h>
SoftwareSerial sensorSerial(5, 4); //4 NOT USED

// Reads from the sensor and prints on the serial monitor the latest value
uint8_t serialRxBuf[255];
uint8_t rxBufIdx = 0;
uint8_t temp = 0;

void setup() {
   sensorSerial.begin(9600);
   Serial.begin(9600);

}

void loop() {
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
              uint16_t pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];
              Serial.print(" dal sensore: ");
              Serial.println(pm25);
              memset(serialRxBuf, 0, sizeof(serialRxBuf));
              rxBufIdx = 0;
        }
        Serial.print("\n");
        
   }
}
