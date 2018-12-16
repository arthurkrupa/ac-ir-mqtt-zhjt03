#define SEND_PIN      4 // NodeMCU 4=D2
#define RECV_PIN      14 // NodeMCU 14=D5
#define LED           D0 // NodeMCU builtin LED
#define CHIGO_DEBUG   false

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "hvac.h"
#include "config.h"

HvacController hvac;
HvacState newHvacState;
HvacState oldHvacState;

// Enums for MQTT payloads
char *ac_modes[] = {"auto","cool","dry","heat","fan_only"};
char *fan_modes[] = {"slow","medium","fast","auto"};
char *swing_modes[] = {"horizontal","fixed","natural"};

// MQTT setup
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

// Connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.print("[WIFI] Connecting to ");
  Serial.print(ssid);
  Serial.print("...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, HIGH);
    delay(500);
    Serial.print(".");
    digitalWrite(LED, LOW);
  }
  
  randomSeed(micros());

  Serial.print(" connected (IP: ");
  Serial.print(WiFi.localIP());
  Serial.println(")");
}

// Callback for received MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT] Message arrived: [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Convert payload to different types
  char message_buff[100];
  int j;
  for (j = 0; j<length; j++) {
    message_buff[j] = payload[j];
  }
  message_buff[j] = '\0';
  const char *p_payload = message_buff;
  float got_float = atof(p_payload);
  int got_int = (int)got_float;
  bool got_bool;
  if (got_int == 0 || got_int<0)
    got_bool = 0;
  else
    got_bool = 1;

  // Power topic in
  if (strcmp(topic,topic_power_subscribe)==0) {
    if (got_bool) {
      hvac.turnOn();
      client.publish(topic_power_publish, "1");
    }
    else {
      hvac.turnOff();
      client.publish(topic_power_publish, "0");
    }
  }

  // Temperature topic in
  if (strcmp(topic,topic_temperature_subscribe)==0) {
    if (got_int > 32)
      got_int = 32;
    if (got_int < 16)
      got_int = 16;
    hvac.setTemperatureTo(got_int);
    client.publish(topic_temperature_publish, String(got_int).c_str());
  }

  // Mode topic in
  if (strcmp(topic,topic_mode_subscribe)==0) {
    for (int i=0; i<sizeof(ac_modes); i++) {
      if (strcmp(p_payload,"off")==0) {
        hvac.turnOff();
        client.publish(topic_power_publish, "0");
        client.publish(topic_mode_publish, "off");
        break;
      }
      else if (strcmp(p_payload,ac_modes[i])==0) {
        hvac.setModeTo(static_cast<Mode>(i));
        client.publish(topic_power_publish, "1");
        client.publish(topic_mode_publish, ac_modes[i]);
        client.publish(topic_temperature_publish, String(hvac.getTemperature()).c_str());
        break;
      }
    }
  }

  // Fan topic in
  if (strcmp(topic,topic_fan_subscribe)==0) {
    for (int i=0; i<sizeof(fan_modes); i++) {
      if (strcmp(p_payload,fan_modes[i])==0) {
        hvac.setSpeedTo(static_cast<Speed>(i));
        client.publish(topic_fan_publish, fan_modes[i]);
        break;
      }
    }
  }

  // Swing topic in
  if (strcmp(topic,topic_swing_subscribe)==0) {
    for (int i=0; i<sizeof(swing_modes); i++) {
      if (strcmp(p_payload,swing_modes[i])==0) {
        hvac.setSwingTo(i);
        client.publish(topic_swing_publish, swing_modes[i]);
        break;
      }
    }
  }

}

// Establish MQTT connection
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("[MQTT] Connecting to ");
    Serial.print(mqtt_server);
    Serial.print("...");

    // Attempt to connect
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.print(" connected");
      client.publish(topic_handshake, "hello world");
      client.subscribe(topic_power_subscribe);
      client.subscribe(topic_temperature_subscribe);
      client.subscribe(topic_mode_subscribe);
      client.subscribe(topic_fan_subscribe);
      client.subscribe(topic_swing_subscribe);
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.print(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    Serial.println();
  }
}

void setup()
{
  pinMode(LED, OUTPUT);
  hvac.setup();
  Serial.println("[STATUS] Waiting for IR signals...");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  newHvacState = hvac.checkIR();

  // Fix for initial abnormal values (e.g. temperature = 1073646649)
  // Interrupt if received values are abnormal
  if (newHvacState.temperature > 32)
    return;

  // Check for changes in power
  if (newHvacState.power != oldHvacState.power) {
    client.publish(topic_power_publish, newHvacState.power ? "1" : "0");
    
    // Fix for Home Assistant MQTT HVAC
    // set pseudo-mode "off"
    if (!newHvacState.power)
      client.publish(topic_mode_publish, "off");
  }

  // Check for changes in temperature
  if (newHvacState.temperature != oldHvacState.temperature) {
    char c_temp[3];
    client.publish(topic_temperature_publish, itoa(newHvacState.temperature, c_temp, 10));
  }

  // Check for changes in AC mode
  if (newHvacState.mode != oldHvacState.mode) {
    if (newHvacState.mode < 5)
      client.publish(topic_mode_publish, ac_modes[newHvacState.mode]);
  }

  // Check for changes in fan speed
  if (newHvacState.airSpeed != oldHvacState.airSpeed) {
    if (newHvacState.airSpeed < 4)
      client.publish(topic_fan_publish, fan_modes[newHvacState.airSpeed]);
  }

  // Check for changes in swing mode
  if (newHvacState.swing != oldHvacState.swing) {
    if (newHvacState.swing < 3)
      client.publish(topic_swing_publish, swing_modes[newHvacState.swing]);
  }
  
  // Update entire state
  oldHvacState = newHvacState;
}

