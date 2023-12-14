# IKEA VINDRIKTNING

Within this repository, I gather codes and resources related to modifying the **IKEA VINDRIKTNING**, an affordable air quality sensor capable of measuring PM 2.5 concentration. The sensor indicates air quality through 3 LEDs on the front panel. However, it lacks any Wi-Fi functionality, making it rather limited on its own.

Nevertheless, since it is based on the **PM1006** sensor, it is possible, through a simple hardware modification, to extract data and enhance the product's capabilities. This modification is extensively documented, and some resources are available at the end of this README.

For a deeper understanding of the modification process, please refer to the `README.old.md` file in this repository.

The code within this repository has been written and tested to function on the [az-delivery D1 mini](https://www.az-delivery.de/it/products/d1-mini), a board based on ESP8266 with Wi-Fi capabilities, can be powered at 5V, with compact dimensions allowing it to fit inside the device case.

### SensorToSerial

In the **sensorToSerial** folder, you will find the `sensorToSerial.ino` file, a sketch that, once compiled and uploaded, prints the sensor-detected data to the serial monitor.

### webServer

In the **webServer** folder, you'll find the `webServer.ino` sketch, which includes a basic web server capable of presenting fundamental information, such as the last detected value, the maximum value, and the number of readings. The sketch is currently configured to connect to Wi-Fi using a static IP, but this option can be easily disabled by removing some lines of code.

### toMySql

Within the **toMySql** folder, two subfolders exist:

- **webClient** contains the sketch to enable the ESP8266 to contact the remote server.
- **server** contains the server written in **node.js** that handles requests and writes everything to a MySQL database.

The server folder also includes a **DOCKERFILE** to facilitate deployment via Docker. An interesting application is to collect data in the database and visualize it using **Grafana**. Below is a screenshot of my dashboard. The toMySql folder also contains the `grafana.json` file, which is the configuration of the aforementioned dashboard.

![Screenshot](https://raw.githubusercontent.com/lollo03/IKEA-VINDRIKTNING-mod-by-Lollo/main/docs/grafanaDashboard.png)

## Future Developments:

- Adding other sensors like ENS160 + AHT21 to gather additional data

## Sources:

- [esp8266-vindriktning-particle-sensor (GitHub)](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor)
- [IKEA-Air-Quality-Sensor](https://github.com/3ative/IKEA-Air-Quality-Sensor/tree/main)
- [ESPHOME PM1006](https://esphome.io/components/sensor/pm1006.html?highlight=pm1006)
- [DATASHEET PM1006](https://cdn-learn.adafruit.com/assets/assets/000/122/217/original/PM1006_LED_PARTICLE_SENSOR_MODULE_SPECIFICATIONS-1.pdf?1688148991)
- [DATASHEET PM1006K](http://innovaertech.com/wp-content/uploads/2022/05/PM1006.pdf)
- [How to Convert and Upgrade the IKEA Air Quality Sensor for Home Assistant (YouTube)](https://www.youtube.com/watch?v=YmqtMTO5NVc)
- [IKEA VINDRIKTNING Page](https://www.ikea.com/it/it/p/vindriktning-sensore-della-qualita-dellaria-80515910/)
