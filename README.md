# IKEA VINDRIKTNING

Questo repository raccoglie informazioni riguardante le possibile modifiche effettuabili all'**IKEA VINDRIKTNING** sensore di qualità dell'aria dotato del **PM1006** capace di misurare la concentrazione di PM2.5. È possibile collegare al sensore un sbc o microcontrollore per estenderne le funzionalità. Qua sotto è descritta una possibile implementazione usando arduino/esp8266 e simili.

## Implementazione

Il sensore comunica tramite UART a 9600 boud, per utilizzarlo basta inizializzare la connessione in questo modo:

```c
#include <SoftwareSerial.h>
SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX);
sensorSerial.begin(9600);
```

> Attenzione! Il pin RX deve supportare gli interrupts ([leggi qua](https://docs.arduino.cc/learn/built-in-libraries/software-serial#limitations-of-this-library))

Il messaggio è lungo 64 byte ed ha un checksum calcolato in questo modo:

```c
bool isValidChecksum() { //calcola il checksum e ritorna 1 se valido
        uint8_t checksum = 0;

        for (uint8_t i = 0; i < 20; i++) {
            checksum += serialRxBuf[i];
        }

        if (checksum != 0) {
            Serial.printf("Received message with invalid checksum. Expected: 0. Actual: %d\n", checksum);
        }

        return checksum == 0;
    }
```

In pratica la somma di tutti i byte deve essere 0. ATTENZIONE `uint8_t` è unsigned, questo vuol dire che per essere zero overflowa:

```c
#include <stdio.h>
#include <stdint.h>

int main(int argc, char const *argv[])
{
    uint8_t checksum = 255;
    checksum++;
    printf("%d \n", checksum);
    return 0;
}
```

output: `0`

Per verificare che il messaggio sia valido è necessario verificare l'header che è sempre `16 11 0B`

```c
bool headerValid = serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B;
```

Si può estrarre la concentrazione di PM 2.5 in questo mondo:
`B[5]*256 + B[6]` Dove `B` è il buffer di byte ricevuti. Implementazione:

```c
const uint16_t pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];
```

## NOTE

1. Se si usa il sensore staccato dal MCU originale di IKEA bisogna mandare questo messaggio via UART entro 5 secondi dal boot: `11 02 0B 01 E1`, altrimenti il sensore entra in modalità PWM.
2. Il funzionamento della modalità PWM è documentato su [questo](http://innovaertech.com/wp-content/uploads/2022/05/PM1006.pdf) datasheet che è dedicato alla versione K del sensore, sarebbe da sperimentare per vedere se effettivamente le istruzioni sono valide anche per la versione non k
3. Sarebbe interessante testare se le informazioni riguardanti PM5 PM10 sono presenti anche nel payload della versione non k del sensore ([fonte](http://innovaertech.com/wp-content/uploads/2022/05/PM1006.pdf))
4. Sulla board IKEA è presente anche un sensore di luce che è possibile utilizzare ([fonte1](https://www.youtube.com/watch?v=YmqtMTO5NVc) [fonte2](https://github.com/3ative/IKEA-Air-Quality-Sensor))
5. Potrebbe essere interessante integrare il tutto con il sensore ENS160 + AHT21 per avere anche i valori di CO2, VOC, temperature ed umidità (possibilità di correggere le rilevazioni di PM1006?)
6. Possibilità di utilizzare i LED già disponibili (?)

## Fonti:

- Repo dal quale ho preso parte del codice mostrato in questo README: [esp8266-vindriktning-particle-sensor](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor)
- [IKEA-Air-Quality-Sensor](https://github.com/3ative/IKEA-Air-Quality-Sensor/tree/main)
- [ESPHOME PM1006](https://esphome.io/components/sensor/pm1006.html?highlight=pm1006)
- [DATASHEET PM1006](https://cdn-learn.adafruit.com/assets/assets/000/122/217/original/PM1006_LED_PARTICLE_SENSOR_MODULE_SPECIFICATIONS-1.pdf?1688148991)
- [DATASHEET PM1006K](http://innovaertech.com/wp-content/uploads/2022/05/PM1006.pdf)
- [How to Convert and Upgrade the IKEA Air Quality Sensor for Home Assistant (youtube)](https://www.youtube.com/watch?v=YmqtMTO5NVc)
- [Pagina IKEA VINDRIKTNING](https://www.ikea.com/it/it/p/vindriktning-sensore-della-qualita-dellaria-80515910/)
