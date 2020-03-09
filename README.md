# Word Clock Hungarian
This is my implementation of the Word Clock in Hungarian. Rest of the document is in Hungarian, English version will follow with the final version of the code.

## Változások
0.9
- Kód első verziója, az alap funckiók működnek
- Fényerő szabályzás még nem működik

## Föbb funkciók
- 10x10 mátrixban elhelyezett magyar szavakkal mutatja az aktuális időt 5 perces pontossággal
- Az egyes szavak színe a napszaknak megfelelően, nappal fehér, este kékes, naplementekor piros, napfelkeltekor narancssárgás és között 1 órát átmenet
- A pontos időt NTP szerverről szedi, kézi beállítás nincs
- Minden egész perckor rövid animálás van, amikor 8-10 véletlenszerű betű felvillan véletlenszerű színnel
- MQTT szerverre minden percben küld egy státus üzenetet: RSSI, uptime percben és a érzékelt fényerő
- MQTTn keresztül figyel egy sleep topic-ra, ahova ha 1-es érkezik akkor lekapcsolja a kijelzőt

## Telepítés
- A kód Arudino IDE alatt kell elfordítani, ami ESP8266 alá be van konfigurálva
- Szükséges libraryk: PxMatrix LED MATRIX library by Dominic Buchstaller, Time by Michael Margolis (és persze az ESP alap libraryk, PubSubClient, stb.)
- Fő ino fájlban a 125 sortól van pár konstans amit be kell állítani
  - ssid: wifi SSID
  - password: wifi jelszó
  - mqtt_server: MQTT szerver IPje, vagy maradjon üres ha nem kell
  - mqtt_user: MQTT usernév
  - mqtt_password: MQTT jelszó
  - clientID: MQTT klines ID
  - topicstatus: az MQTT topic amire az óra küldi percenként a státusz üzenetet
  - topicDebug: az MQTT topic amire a debug üzenetek mennek
  - topicSleep: az MQTT topic amire várja a sleep üzentetet
- NTP.h fájl 63 sorától pár beállítás
  - GMTOffset: időzóna percben CET esetén ez 60
  - sm_latitude, sm_longtitude: földrajzi szélesség és hosszúság a napfelkelte, napnyugta számításhoz
- Arduino Board Configuration amit én használtam: LOLIN(WEMOS) D1 R2 mini, 4MB Flash size

## PCB
Minden részletesebb leírás nélkül egyelőre itt az általam használt PCB: https://www.pcbway.com/project/shareproject/PxMatrix_ESP8266_Driver.html
img könyvtárban van egy kép hogy néz ki. Részletes leírás majd később.

## Hardver
- 32x32 pixeles, 192x192 mm P6 matrix kijelző: https://www.aliexpress.com/item/32658820147.html
- Wemos D1 mini klón: ebay, aliexpress, banggood, én már nem emlékszem az enyém honnan van
- 0.1 inch-es tűsor és csatlakozó 
- jumperek
- 0.2 inch-es csavaros 2 pólusú csatlakozó
- fotóellenállás

A PI és PO csatlakozóhoz nem vettem szabványos dugót és ajzatot, hanem sima egy soros tűsort és csatlakozót használtam, kéttőt egymás mellett és tökéletesen működik. Sőt a PO és a tápcsatlakozó olyan közel van egymáshoz hogy ott nem és férne el rendes ajzat. Viszont cserébe figyelni kell hogy dugja be az ember a dolgokat. Ahogy a képen is látszik a kijelzőn nyák lapján lévő nyilacskák balról jobbra mutassanak, és a PO csatlakozóba a szalagkábelt úgy kell bedugni hogy a csatlakozó oldalán a kiugrás a ESP felé legyen.

![Finished PCB](img/20200307_102507.jpg)

A nyákon a táp csatlakozó 5V és GND vezetéke össze van kötve a ESP 5V és GND lábával, így lehet a kijelzőt az ESPn keresztül microUSB csatlakozón keresztül hajtani, vagy más tápellátás esetén azt is a tápcsatlakozóra kötni és akkor azt ad áramot az ESPnek. Az én programom esetén ahol egyszerre sosem világít túl sok LED, USB tápról lehet a kijelzőt hajtani.
A P6-os matrix kijelző esetén a jumpereket a fenti képen látható módon kell elhelyezni.

## 3D nyomtatott alkatrészek
Ezek sem véglegesek. Ezek csak azért kellenek hogy az egyes LEDek annyira ne látszódjanak. Minden szükséges fájl ott van a 3D alkönyvtárban.
