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
-- ssid: wifi SSID
-- password: wifi jelszó
-- mqtt_server: MQTT szerver IPje, vagy maradjon üres ha nem kell
-- mqtt_user: MQTT usernév
-- mqtt_password: MQTT jelszó
-- clientID: MQTT klines ID
-- topicstatus: az MQTT topic amire az óra küldi percenként a státusz üzenetet
-- topicDebug: az MQTT topic amire a debug üzenetek mennek
-- topicSleep: az MQTT topic amire várja a sleep üzentetet
- NTP.h fájl 63 sorától pár beállítás
-- GMTOffset: időzóna percben CET esetén ez 60
-- sm_latitude, sm_longtitude: földrajzi szélesség és hosszúság a napfelkelte, napnyugta számításhoz
- Arduino Board Configuration amit én használtam: LOLIN(WEMOS) D1 R2 mini, 4MB Flash size

## PCB
