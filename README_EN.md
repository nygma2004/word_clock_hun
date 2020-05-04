# Word Clock Hungarian
This is my implementation of the Word Clock in Hungarian. I post a long Youtube video on this project if you are interested. Even though I did this project in Hungarian, the code can be easliy adapted to any language. And as long as you can also fit in a 10x10 matrix, it is even easier.

## Change Log
1.0
- Settings and other parameters moved to separate files
- Brigthness control of the display

0.9
- First version of the code, main features included
- Brightness control missing

## Main features
- Display the current time by ligthing better in a 10x10 matrix which spell out the correct time. Every 5 minutes increment in Hungarian.
- The colour of the letters follows the time of the day, it is blue overnight, orange in dawn, white during the day, red at sunset. There is a 1 hour transition for each phased and the colour transition from one to another.
- Time is synced from NTP server, there is no manual setting.
- There is a small animation every minute, where 8-10 letters are light randomly in a random color.
- Status message is sent over MQTT which includes the RSSI, uptime and the brightness value.
- Device subscribes to a sleep topic in MQTT and if 1 is received, the screen is turned off ("quiet" mode overnight).

## Installing
- This is an Arduino IDE sketch, the IDE needs to be configured for ESP8266.
- Required libraries: PxMatrix LED MATRIX library by Dominic Buchstaller, Time by Michael Margolis (all can be found in the Library Manager), and basic ESP libraries like HTTP, PubSubClient for MQTT stb.
### Project files:
- WordClock_PxMatrix.ino: full source code. If the project is re-implemented for another language, some changes may be required here. More on that in the video.
- settings.h: contains all paramters and program settings
  - ANALOG_LOW: lowest A0 analogue value, which corresponds to darkness
  - BRIGHTNESS_LOW: brightness value of the display fo the lowest ANALOG_LOW value
  - ANALOG_HIGH 120: highest A0 analogue value, which corresponds to brightess condition
  - BRIGHTNESS_HIGH: brightness value of the display fo the lowest ANALOG_HIGH value
  - ssid: wifi SSID
  - password: wifi password
  - mqtt_server: MQTT server IP, leave empty to disable MQTT
  - mqtt_user: MQTT user name
  - mqtt_password: MQTT password
  - clientID: MQTT client ID
  - topicstatus: MQTT topic to which the status messages will be sent to
  - topicDebug: MQTT topic to which the debug messages will be sent to
  - topicSleep: MQTT sleep topic the device subscribes to
  - STATUS_UPDATE_INTERVAL: interval of the status messages in seconds
  - ANIMATION_WORD_COUNT: number of letters in the animation every minute
  - ANIMATION_WORD_DELAY: delay between each letter in the animation, in miliseconds
- NTP.h: NTP code and there are a few settings from line 63
  - GMTOffset: offset from GMT in minutes. For CET it should be set to 60.
  - sm_latitude, sm_longtitude: Your latitude and longtitude for the sunrise, sunset calculation
- Arduino Board Configuration: LOLIN(WEMOS) D1 R2 mini, 4MB Flash size

## PCB
This the PCB I designed for this project, and what you see in the pictures below: https://www.pcbway.com/project/shareproject/PxMatrix_ESP8266_Driver.html
More details in the video.

## Hardware
- 32x32 pixel, 192x192 mm P6 matrix display: https://www.aliexpress.com/item/32658820147.html
- Wemos D1 mini clone: ebay, aliexpress, banggood, I purchased mine a long time ago
- 0.1 inch male and female pin header 
- jumpers
- 0.2 inch pich terminal block
- LDR

I did not use the proper keyed 2x5 headers for the PI and PO connectors, I simply used two single pin headers next to each other. And the board works fine, just pay attention how it gets plugged in. In fact the PO header and the power terminal block is so close to each other that a proper male header does not fit. Check how it is plugged in as shown on the picture below. Pay attention to the arrows on the matrix PCB, it should be pointing up and right. The ribbon cable should have the key facing the ESP.

![Finished PCB](img/20200307_102507.jpg)

The PCB contains a 5V and GND power connection which is the terminal block. Those terminals are connected to the 5V and GND pins of the ESP. The display gets the power from the terminal blocks where the provided power cable plugs to. The setup can either be powered from the micro USB of the ESP, or in case of a separate 5V power supply, it should be connected terminal blocks. In both cases ESP and the display will receive power. Even though these displays can draw multiple amps when all pixels are light, in this case the usual consumption is around 150-300 mA. I am powering mine from a decent 1A USB wall charger to have some margin.

## Brightness Control

There is a place for a photoresistor (LDR) and a pull-down resistor on the board. I soldered my LDR without cutting the leads (insulted the leads with heatsrink tubing) and bent it out to measure the light on the side. I purchased an LDR with a resistor of 5K at full sunlight. For that I used a 5K resistor (where it says 10K on the silk screen). This does not give me the full range of analogue values, but still good enough to conrol the screen brightness:

![Finished PCB](img/20200503_140058.jpg)

With this the max brigtness value I get is around 400. Set the 4 brightness values in the settings.h to control the display. Pick values which works best for you.

## 3D printed parts and clock face
All 3D printed parts can be found in the 3D folder. There are multiple parts to the complete construction of the clock. You find the STL and gcode files as well.
- 10x10 raster: this goes on the screen to box the pixels into a 3x3 raster for each letter
- 4 corners: these corners go on the front of the display. These are separate parts because otherwise the model would have been too big for my 3D printer. Note that all 3 coreders are different, as the groves on the display are not the same in the corners.
- Lower corners: these corners keep the raster in place for the other side. They also have feeds to keep the display away from the wall to clear the electronics behind.
- Hanger support: small cone for hanging the clock on the wall
- Cable holder: keeps the cable along the wall and also acts as a strain relief so the cable does not pull on the ESP

Here you see the raster and the 4 corners which are superglued together:
![Finished PCB](img/20200503_140026.jpg)

One of the corner in a closeup, and you can see how the raster and the corner fits in the grooves and ridges of the display:
![Finished PCB](img/20200503_140035.jpg)

Parts at the back:
![Finished PCB](img/20200503_140046.jpg)

My clock face is 1.5 mm thick mild steel laser cut and hand sanded with 150 grit sandpaper for a brushed effect. Finally it was spray painted with clear laquer to prevent rust. You find the pdf and dwg drawing in the design folder which I sent to the laser cutting company to cut mine. Tracing paper goes in between the clockface and the raster to further blend the light from the individual pixels.
