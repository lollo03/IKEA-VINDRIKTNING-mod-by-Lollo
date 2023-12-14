# IKEA VINDRIKTNING

In questo repository raccolgo codici e risorse riguardanti la modifica dell'**IKEA VINDRIKTNING**, sensore di qualità dell'aria economico capace di misurare la concentrazione di PM 2.5 . Il sensore mostra la qualità dell'aria tramite 3 led posti sulla parte frontale. Tuttavia manca qualsiasi tipo di funzionalità wifi, quindi di per se è abbastanza inutile.

Tuttavia essendo basato sul sensore **PM1006** è possibile tramite una semplice modifica hardware estrarre i dati e potenziare le funzionalità del prodotto. Questa modifica è ampiamente documentata, alcune risorse sono disponibili alla fine di questo README.

Maggiori informazioni sul funzionamento della modifica sono disponibili nel file `README.old.md` in questa repository.

Il codice di questo repository è stato scritto e testato per funzionare sull'[az-delivery D1 mini](https://www.az-delivery.de/it/products/d1-mini), board basata su esp8266 dotata di wifi, alimentabile a 5V e di piccole dimensioni, in modo da poter essere collocata all'interno del case del dispositivo.

### SensorToSerial

Nella cartella **sensorToSerial** è presente il file `sensorToSerial.ino` uno sketch che una volta compilato e caricato si occupa di stampare sul monitor seriale i dati rilevati dal sensore.

### webServer

Nella cartella **webServer** è presente il file `webServer.ino` sketch che presenta un rudimentale web server capace di presentare alcune basiche informazioni, come l'ultimo valore rilevato, il valore massimo ed il numero di rilevazioni. Lo sketch al momento è configurato per connettersi al WiFi tramite IP statico, ma è possibile disabilitare questa opzione facilmente eliminando alcune linee di codice.

### toMySql

All'interno della cartella **toMySql** sono presenti due sottocartelle.

- **webClient** contiene lo sketch per permettere all'esp 8266 di contattare il server remoto
- **server** server contiene il server scritto in **node.js** che si occupa di gestire le richieste e scrivere il tutto su un database mysql

Nella cartella server è presente anche un **DOCKERFILE** per facilitare il deployment tramite docker. Un'applicazione interessante è quella di raccogliere i dati nel database e visualizzarli usando **grafana**, qua sotto è presente uno screen shoot della mia dashboard. Nella cartella toMySql è presente anche il file `grafana.json` che è la configurazione della dashboard appena descritta.

![Screen shoot](https://raw.githubusercontent.com/lollo03/IKEA-VINDRIKTNING-mod-by-Lollo/main/docs/grafanaDashboard.png)

## Sviluppi futuri:

- aggiungere altri sensori come ENS160 + AHT21 per raccogliere ulteriori dati

## Fonti:

- [esp8266-vindriktning-particle-sensor (github)](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor)
- [IKEA-Air-Quality-Sensor](https://github.com/3ative/IKEA-Air-Quality-Sensor/tree/main)
- [ESPHOME PM1006](https://esphome.io/components/sensor/pm1006.html?highlight=pm1006)
- [DATASHEET PM1006](https://cdn-learn.adafruit.com/assets/assets/000/122/217/original/PM1006_LED_PARTICLE_SENSOR_MODULE_SPECIFICATIONS-1.pdf?1688148991)
- [DATASHEET PM1006K](http://innovaertech.com/wp-content/uploads/2022/05/PM1006.pdf)
- [How to Convert and Upgrade the IKEA Air Quality Sensor for Home Assistant (youtube)](https://www.youtube.com/watch?v=YmqtMTO5NVc)
- [Pagina IKEA VINDRIKTNING](https://www.ikea.com/it/it/p/vindriktning-sensore-della-qualita-dellaria-80515910/)
