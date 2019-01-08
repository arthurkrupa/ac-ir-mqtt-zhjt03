#define SEND_PIN      15 // NodeMCU 15=D8
#define RECV_PIN      14 // NodeMCU 14=D5
#define DEBUG_MODE    false // Dump debugging info to serial monitor
#define MEMORY_MODE   true // Save HVAC state in EEPROM
#define MEMORY_INIT   false // Run only once on new device to prepare EEPROM

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* clientID = "ZHJT-03";
const char* topic_handshake = "my_topic/handshake";
const char* topic_power_publish = "my_topic/power/get";
const char* topic_power_subscribe = "my_topic/power/set";
const char* topic_temperature_publish = "my_topic/temperature/get";
const char* topic_temperature_subscribe = "my_topic/temperature/set";
const char* topic_mode_publish = "my_topic/mode/get";
const char* topic_mode_subscribe = "my_topic/mode/set";
const char* topic_fan_publish = "my_topic/fan/get";
const char* topic_fan_subscribe = "my_topic/fan/set";
const char* topic_swing_publish = "my_topic/swing/get";
const char* topic_swing_subscribe = "my_topic/swing/set";