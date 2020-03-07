// Word Clock Hungarian
// Repo: https://github.com/nygma2004/word_clock_hun
// author: Csongor Varga, csongor.varga@gmail.com

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>           // MQTT support
#include <WiFiUdp.h>                // Added for NTP functionality
#include "NTP.h"

#include <PxMatrix.h>
#include <Time.h>
#include <TimeLib.h>
#include <sunMoon.h>                // Sunrise, sunset calculation
extern "C" {
#include "user_interface.h"
}

#define PxMATRIX_COLOR_DEPTH 8
#define PxMATRIX_MAX_HEIGHT 32
#define PxMATRIX_MAX_WIDTH 32
#define MINSIZE 19
#define HOURSIZE 11

#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2
#define DEBUG 1

#endif

// PxMatrix definition
//PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX display(32,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

// This stores the pixels to be light for each hour. 
const byte hours[13][HOURSIZE] = {
  // nulla ora
  {},
  // egy ora
  {75,76,77,255},
  // ketto ora
  {85,86,87,88,89,255},
  // harom ora
  {55,56,57,58,59,255},    
  // negy ora
  {50,51,52,53,255}, 
  // ot ora
  {65,66,255}, 
  // hat ora
  {47,48,49,255}, 
  // het ora
  {67,68,69,255}, 
  // nyolc ora
  {60,61,62,63,64,255}, 
  // kilenc ora
  {90,91,92,93,94,95,255}, 
  // tiz ora
  {70,71,72,255}, 
  // tizenegy ora
  {70,71,72,73,74,75,76,77,255}, 
  // tizenketto ora
  {80,81,82,83,84,85,86,87,88,89,255}
  };

// This stores the pixels to be light for each 5 minute interval. 
const byte minutes[12][MINSIZE] = {
  // 00 minute
  {97,98,99,255},
  // 05 minute
  {0,1,10,11,12,13,14,15,16,20,21,22,23,97,98,99,255},
  // 10 minute
  {3,4,5,10,11,12,13,14,15,16,20,21,22,23,97,98,99,255},
  // 15 minute
  {40,41,42,43,44,45,255},
  // 20 minute
  {3,4,5,10,11,12,13,25,26,27,28,29,36,37,38,255},
  // 25 minute
  {0,1,10,11,12,13,25,26,27,28,29,36,37,38,255},
  // 30 minute
  {36,37,38,255},
  // 35 minute
  {0,1,10,11,12,13,14,15,16,20,21,22,23,36,37,38,255},
  // 40 minute
  {3,4,5,10,11,12,13,14,15,16,20,21,22,23,36,37,38,255},
  // 45 minute
  {30,31,32,33,34,40,41,42,43,44,45,255},
  // 50 minute
  {3,4,5,10,11,12,13,25,26,27,28,29,97,98,99,255},
  // 55 minute
  {0,1,10,11,12,13,25,26,27,28,29,97,98,99,255}
};

// Update the below parameters for your project
// Also check NTP.h for some parameters as well
const char* ssid = "NygmaRinglo37";
const char* password = "Ringlo2012Oliver";
const char* mqtt_server = "192.168.1.80"; 
const char* mqtt_user = "mquser";
const char* mqtt_password = "mokus4";
const char* clientID = "wordclock";
const char* topicStatus = "/wordclock/status";
const char* topicDebug = "/wordclock/debug";
const char* topicSleep = "/sleep";

String NTPtime = "--:--";
unsigned long lastTick, uptime, lastNTP, seconds, epoch2, sec, lastANI;
byte LEDR, LEDG, LEDB;
byte updateScreen, aniSequence; 
int clockhour, clockfivemin;
time_t sRise;
time_t sSet;
String mqttStat = "";
String message;
int brightnessAnalog = 0;
os_timer_t myTimer;

MDNSResponder mdns;
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient mqtt(mqtt_server, 1883, 0, espClient);

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(70);
  // display.displayTestPixel(70);
  display.display();
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  //isplay.display(70);
  display.displayTestPattern(70);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif


// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(128, 128, 128);

uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myYELLOW,myCYAN,myMAGENTA,myBLACK,myWHITE};
int i =0;

void setup() {
    // Serial setup for debug messages
    Serial.begin(9600);
    Serial.println();
    Serial.println("Word Clock - Hungarian");
  
    // Px Matrix display setup
    display.begin(8);
    display.setFastUpdate(true);
    display.setScanPattern(WZAGZIG);
    display.setColorOrder(BBGGRR);
    display.setBrightness(70);
    display.flushDisplay();
  
    uptime = 0;
    sec = 0;
    seconds = -1;
    updateScreen = 0;
    aniSequence = 0;
  
  
    // interrupt setup for the display
    #ifdef ESP8266
      display_ticker.attach(0.002, display_updater);
    #endif
  
    #ifdef ESP32
      timer = timerBegin(0, 80, true);
      timerAttachInterrupt(timer, &display_updater, true);
      timerAlarmWrite(timer, 2000, true);
      timerAlarmEnable(timer);
    #endif

    Serial.print(F("Connecting to Wifi"));
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
      seconds++;
      drawWordPixel(seconds%10,seconds/10,myRED);
      if (seconds>180) {
        // reboot the ESP if cannot connect to wifi
        ESP.restart();
      }
    }

    Serial.println("");
    Serial.print(F("Connected to "));
    Serial.println(ssid);
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("Signal [RSSI]: "));
    Serial.println(WiFi.RSSI());

    // Set up the MDNS and the HTTP server
    if (mdns.begin("Word Clock", WiFi.localIP())) {
      Serial.println(F("MDNS responder started"));
      seconds++;
      drawWordPixel(seconds%10,seconds/10,myYELLOW);
    }  
    server.on("/", [](){                        // Dummy page
      server.send(200, "text/plain", "Word Clock");
    });
    server.begin();
    Serial.println(F("HTTP server started"));
    seconds++;
    drawWordPixel(seconds%10,seconds/10,myYELLOW);

    // Start UDP for NTP function
    Serial.println(F("Starting UDP"));
    udp.begin(localPort);
    Serial.print(F("Local port: "));
    Serial.println(udp.localPort());
    seconds++;
    drawWordPixel(seconds%10,seconds/10,myGREEN);
    requestNTPUpdate();
    // Initialize sunrise and sunset calculator
    sm.init(GMTOffset, sm_latitude, sm_longtitude);  
    seconds++;
    drawWordPixel(seconds%10,seconds/10,myGREEN);
  
    // Update the screen brightness
    // TEST
    //BrightnessCheck(); 

    // Timer interrupt for the clock functions
    os_timer_setfn(&myTimer, timerCallback, NULL);
    os_timer_arm(&myTimer, 1000, true);

    // Set up the MQTT server connection
    if (mqtt_server!="") {
      mqtt.setServer(mqtt_server, 1883);
      mqtt.setCallback(MQTTcallback);
      reconnect();
    }
    seconds++;
    drawWordPixel(seconds%10,seconds/10,myYELLOW);
    randomSeed(millis());
    seconds = 0;



}

// MQTT reconnect logic
void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(clientID, mqtt_user, mqtt_password)) {
      Serial.println(F("connected"));
      // ... and resubscribe
      mqtt.subscribe(topicSleep);
      Serial.print(F("Subscribed to "));
      Serial.println(topicSleep);
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(mqtt.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// This routing just puts the status string together which will be sent over MQTT
void refreshStats() {
  // Initialize the strings for MQTT 
  mqttStat = "{\"rssi\":";
  mqttStat += WiFi.RSSI();
  mqttStat += ",\"uptime\":";
  mqttStat += uptime;
  mqttStat += ",\"brightness\":";
  mqttStat += brightnessAnalog;
  mqttStat += "}";
}

void BrightnessCheck() {
  // Check for the photoresistor and set the brightness level
  brightnessAnalog = analogRead(0);
  if (brightnessAnalog<100) {
    display.setBrightness(10);
  } else if(brightnessAnalog<400) {
    display.setBrightness(20);
  } else if(brightnessAnalog>400) {
    display.setBrightness(40);
  }  
}

void handleNTPResponse() {
  // Check for NTP response
  int cb = udp.parsePacket();
  if (cb!=0) {
    if (NTPUpdateMillis == 0) {
      // This is only true after boot
      //currentPage = 0xff; // set the clock page
      //pageChange = true; // transition in the clock page
    }
    NTPUpdateMillis = millis();
    NTPRequested = false;
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    epoch = secsSince1900 - seventyYears;
    if (summerTime(epoch)) {
      epoch += 3600; // add 1 hour DST
    }
    epoch += GMTOffset * 60;
    // TEST DATA
    //epoch = 1548396000;
    Serial.print(F("NTP Update (epoch): "));
    Serial.println(epoch);
    Serial.print(F("NTP Update (time): "));
    printDate(epoch);  Serial.println("");

    // Sunrise and Sunset calculations
    sRise = sm.sunRise(epoch);
    sSet  = sm.sunSet(epoch);
    if (summerTime(epoch)) {
      sRise += 3600; // add 1 hour DST
      sSet += 3600; // add 1 hour DST
    }
    renderTime();
    Serial.print(F("Today sunrise and sunset: "));
    Serial.print(sRise); Serial.print(F("; "));
    Serial.print(sSet);  Serial.println("");
    //printDate(sRise); Serial.print(F("; "));
    //printDate(sSet);  Serial.println("");
    if (updateScreen==0) {
      // go to the time screen since now we have the correct time - only in the beginning
      updateScreen = 1;
    }
  }  
}


// HTTP request handle for the page update
void handleUpdateCommand() { 
  message = "Word Clock";
  server.send(200, "text/plain", message);          //Returns the HTTP response
}

// MQTT callback function
void MQTTcallback(char* topic, byte* payload, unsigned int length) {
  // Convert the incoming byte array to a string
  String strTopic = String((char*)topic);
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;

  Serial.print(F("Message arrived on topic: ["));
  Serial.print(strTopic);
  Serial.print(F("], "));
  Serial.println(message);

  if (strTopic==(String)topicSleep) {
    Serial.print(F("Sleep: "));
    if (atoi((char *)payload)==1) {
      // Sleep mode starts
      updateScreen = 200;
      display.clearDisplay();
      Serial.print(F("start"));
    } else {
      updateScreen = 1;
      Serial.print(F("end"));
    }
    Serial.println();
  }
}

void calculateColors() {
int trans = 3600; // transition period in and out of sunrise and sunset (1 hour)
  // the current NTP corrected time is stored in epoch2. It is assumed that it is updated by the flow
String state = "";
String msg = "";
  if((epoch2>=sRise+trans)&&(epoch2<=sSet-trans)) {
    // it is day time now (1 hour after sunrise and 1 hour before sunset
    state = "daytime";
    LEDR = 255;
    LEDG = 255;
    LEDB = 255;
  } else {
    if((epoch2<=sRise-trans)||(epoch2>=sSet+trans)) {
      // it is night time now  (1 hour after sunset 1 hour hour before sunset
      state = "nighttime";
      LEDR = 76;
      LEDG = 103;
      LEDB = 255;      
    } else {
      if((epoch2>sRise-trans)&&(epoch2<=sRise)) {
        // transitioning from night to sunrise. Sunrise: 255,193,94
        state = "night to sunrise";
        float phase = ((float)sRise - (float)epoch2 ) / 3600; // 1 in the beginning and reduces to 0
        LEDR = 255-(int) ((255-76)*phase);
        LEDG = 193-(int) ((193-103)*phase);
        LEDB =94+(int) ((255-94)*phase);
      }
      if((epoch2>sRise)&&(epoch2<sRise+trans)) {
        // transitioning from sunrise to daytime
        float phase = ((float)epoch2 - (float)sRise) / 3600; // 0 in the beginning and increases to 1
        state = "sunrise to daytime";
        LEDR = 255;
        LEDG = 193+(int) ((255-193)*phase);
        LEDB = 94+(int) ((255-94)*phase);
      }
      if((epoch2>sSet-trans)&&(epoch2<=sSet)) {
        // transitioning from daytime to sunset. Sunset: 255,90,68
        float phase = ((float)sSet - (float)epoch2) / 3600; // 1 in the beginning and decreases to 0
        state = "daytime to sunset";
        LEDR = 255;
        LEDG = 90+(int) ((255-90)*phase);
        LEDB = 68+(int) ((255-68)*phase);
      }
      if((epoch2>sSet)&&(epoch2<sSet+trans)) {
        // transitioning from sunset to nighttime
        float phase = ((float)epoch2 - (float)sSet) / 3600; // 0 in the beginning and increases to 1
        state = "sunset to nighttime";
        LEDR = 255-(int) ((255-76)*phase);
        LEDG = 90+(int) ((103-90)*phase);
        LEDB = 68+(int) ((255-68)*phase);
      }
    }
  }
  msg = NTPtime + String(" | ") + state + String(" | ") + LEDR + String(",") + LEDG + String(",") + LEDB;
  Serial.println(msg);
  #ifdef DEBUG
    if (mqtt_server!="") {
      mqtt.publish(topicDebug, msg.c_str());
    }
  #endif
}

void renderTime() {
  // Construct the current time string
  if (NTPUpdateMillis>0) {
    NTPtime  = "";
    // TEST
    //epoch2 = epoch;
    epoch2 = epoch+((millis()-NTPUpdateMillis) / 1000);
    if (((epoch2 % 86400L) / 3600)<10) {
      // add leading zero if hour is less than 10
      NTPtime  += "0";
    }
    NTPtime  += (epoch2 % 86400L) / 3600; // print the hour (86400 equals secs per day)
    NTPtime  += ":";
    if ( ((epoch2 % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      NTPtime  += "0";
    }
    NTPtime  += (epoch2  % 3600) / 60; // print the minute (3600 equals secs per minute)
  } else {
    NTPtime = "--:--";
  }  
}

// This is the 1 second timer callback function
void timerCallback(void *pArg) {
  sec++;
  seconds++;
  if ((sec % 60==0)&&(updateScreen!=2)&&(updateScreen!=200)) {
    updateScreen = 2;
    aniSequence = 0;
  }
  if (seconds==10) {
    // Send MQTT update
    refreshStats();
    if (mqtt_server!="") {
      mqtt.publish(topicStatus, mqttStat.c_str());
      Serial.print(F("Status: "));
      Serial.println(mqttStat);
    }
    seconds = 0;
  }
}

//Bubble sort of an array
void isort(byte *a, int n)
{
 for (int i = 1; i < n; ++i)
 {
   int j = a[i];
   int k;
   for (k = i - 1; (k >= 0) && (j < a[k]); k--)
   {
     a[k + 1] = a[k];
   }
   a[k + 1] = j;
 }
}

// This method is responsible for updating the words on the clockface. It also does the transitioning in of the words
// this should only be called when the clockface needs to change (every 5 minutes)
void updateWords() {
  unsigned long temp;
  String msg = "";
  temp = epoch2; // current time in seconds calculated from the NTP time
  temp = epoch2 / 60; // convert to minutes
  clockfivemin = temp % 60 / 5; // convert to 5 minute time
  clockhour = temp / 60 % 24; // convert to hour of the day
  msg = String("hour=") + clockhour + String(", 5min=") + clockfivemin;
  // convert 24h hours to 12h hours
  if (clockhour > 12) {
    clockhour = clockhour - 12; 
  }
  // the below statement is required as from quarter past you say the next hour (in Hungarian)
  if (clockfivemin>2) {
    clockhour++;
    if (clockhour == 13) clockhour = 1;  // handle roll over beyond 12 o'clock
  }
  if (clockhour == 0) {
    clockhour = 12; // zero hour is called 12 o'clock
  }
  msg += String(" -> hour=") + clockhour + String(", 5min=") + clockfivemin;
  
  byte pixels[50]; // this will be my temporary array to hold all the pixels for the clockface
  int pixelCount = 0;
  // copy all pixels from the hour array to the pixels array
  msg += String(" | H=");
  for(int i=0;i<HOURSIZE;i++) {
    if (hours[clockhour][i]==255) break;
    pixels[pixelCount] = hours[clockhour][i];
    msg += pixels[pixelCount] + String(",");
    pixelCount++;
  }
  // copy all pixels from the minute array to the pixels array
  msg += String(" | M=");
  for(int i=0;i<MINSIZE;i++) {
    if (minutes[clockfivemin][i]==255) break;
    pixels[pixelCount] = minutes[clockfivemin][i];
    msg += pixels[pixelCount] + String(",");
    pixelCount++;
  }

  // sort the array
  isort(pixels,pixelCount);

  Serial.println(msg);
  #ifdef DEBUG
    if (mqtt_server!="") {
      mqtt.publish(topicDebug, msg.c_str());
    }
  #endif
    
  uint16_t currentColor = display.color565(LEDR, LEDG, LEDB);
  // display all pixels one by one
  display.clearDisplay();
  for(int i=0;i<pixelCount;i++) {
    drawWordPixel(pixels[i]%10,pixels[i]/10,currentColor);
    
    yield();
    delay(40);
  }
}

void updateAnimation() {
  //display.clearDisplay();
  //Serial.println("Animation starts...");
  //for(int i=0;i<10;i++) {
    int randNumber = random(100);
    drawWordPixel(randNumber%10,randNumber/10,myCOLORS[clockfivemin % 6]);
    aniSequence++;
  //}
}

void drawWordPixel(int16_t x, int16_t y, uint16_t color) {
    // update 3x3 pixels for each word, leaving the first row and column empty
    display.drawPixel(1 + x*3, 1 + y*3, color);
    display.drawPixel(1 + x*3, 2 + y*3, color);
    display.drawPixel(1 + x*3, 3 + y*3, color);
    display.drawPixel(2 + x*3, 1 + y*3, color);
    display.drawPixel(2 + x*3, 2 + y*3, color);
    display.drawPixel(2 + x*3, 3 + y*3, color);
    display.drawPixel(3 + x*3, 1 + y*3, color);
    display.drawPixel(3 + x*3, 2 + y*3, color);
    display.drawPixel(3 + x*3, 3 + y*3, color);  
}



void loop() {

  // Handle HTTP server requests
  server.handleClient();

  // Uptime calculation
  if (millis() - lastTick >= 60000) {            
    lastTick = millis();            
    uptime++;            
  }      

  // Check if NTP update is due
  if ((millis() - NTPUpdateMillis >= 60*60*1000) && (!NTPRequested)) {  
    requestNTPUpdate();
  }    

  handleNTPResponse();

  // Handle MQTT connection/reconnection
  if (mqtt_server!="") {
    if (!mqtt.connected()) {
      reconnect();
    }
    mqtt.loop();
  }

  // regular clock display, light up the words
  if (updateScreen==1) {
    renderTime();
    calculateColors();
    updateWords();
    updateScreen = 0;
  }

  // random word animation every minute
  if (updateScreen==2) {
    if (millis()-lastANI>100) {
      if (aniSequence==0) {
        display.clearDisplay();
      }
      if (aniSequence>9) {
        updateScreen = 3;
        lastANI = millis();
        aniSequence = 0;
      } else {
        updateAnimation();
        lastANI = millis();
      }
    }
  }

  // pausing the screen for a second after the word animation
  if (updateScreen==3) {
    if (millis()-lastANI>1000) {
        updateScreen = 1;
        lastANI = millis();
        aniSequence = 0;
    }
  }

}
