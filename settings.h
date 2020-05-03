// Word Clock Hungarian
// Repo: https://github.com/nygma2004/word_clock_hun
// author: Csongor Varga, csongor.varga@gmail.com

// Review these settings and change the values according to your setup

#define ANALOG_LOW 0         // lowest value from the A0 input
#define BRIGHTNESS_LOW 50    // lowest brightness value
#define ANALOG_HIGH 120      // highest value from the A0 input
#define BRIGHTNESS_HIGH 255  // highest brightness value

// Update the below parameters for your project
// Also check NTP.h for some parameters as well
const char* ssid = "xxx";                               // SSID of your Wifi network
const char* password = "xxx";                           // Password for your wifi network
const char* mqtt_server = "192.168.1.xx";               // MQTT server address, leave empty to disable MQTT function
const char* mqtt_user = "xxx";                          // MQTT user id
const char* mqtt_password = "xxx";                      // MQTT password
const char* clientID = "wordclock";                     // MQTT client ID
const char* topicStatus = "/wordclock/status";          // MQTT topic where the device sends updates every 10 seconds
const char* topicDebug = "/wordclock/debug";            // MQTT topic for debug messeges
const char* topicSleep = "/sleep";                      // MQTT topic the device listens to go to sleep mode (payload 1)

#define STATUS_UPDATE_INTERVAL 10       // Number of second between status update messages
#define ANIMATION_WORD_COUNT 10         // Number of letter to show during animation
#define ANIMATION_WORD_DELAY 100        // Delay between word lit in the word animation (in miliseconds)
