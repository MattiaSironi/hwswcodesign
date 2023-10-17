#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>

#define SERVER_ON 0

enum state_enum {NORMAL, ALERT, FAULT, DEBUG};
state_enum state[1]; 
WiFiClient wifi_client;
MQTTClient mqtt_client;
static const char *ssid = WIFI_SSID;
static const char *wifi_password = WIFI_PASSWORD;
static const char *host = MQTT_HOST;
static const int port = MQTT_PORT;
static const char *username = MQTT_USERNAME;
static const char *mqtt_password = MQTT_PASSWORD;
unsigned long lastMillis = 0;

void messageCallback(String &topic, String &payload);

void set_up_network() {
  SerialUSB.println("Setting up network connection");

  WiFi.begin(ssid, wifi_password);
  SerialUSB.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    SerialUSB.print(".");
    delay(1000);
  }
  SerialUSB.println("\nWiFi connection successful!");

  mqtt_client.begin(host, port, wifi_client);
  mqtt_client.onMessage(messageCallback);
  SerialUSB.println("Connecting to MQTT broker");
  while (!mqtt_client.connect("mkr1000", username, mqtt_password)) {
    SerialUSB.print(".");
    delay(500);
  }
  SerialUSB.println("\nMQTT connection successful!");
}

void messageCallback(String &topic, String &payload) {
  SerialUSB.println("Received message '"+payload+"' from topic "+topic);
}

void setup() {
  Serial1.begin(9600);
  while (!Serial.available());
  if(SERVER_ON) {
    set_up_network();
    mqtt_client.subscribe("teamE/test/receive");
  }
}

void loop() {
  while(!Serial1.available());
  String msg = Serial1.readString();
  int id, max_t, min_t, avg_t;
  state_enum state_tmp;
  char msg_str[15];
  msg.toCharArray(msg_str, 15);
  sscanf(msg_str, "%d,%d,%d,%d,%d", &id, &state_tmp, &max_t, &min_t, &avg_t);
  state[id] = state_tmp;
  if (SERVER_ON) {
    mqtt_client.loop();
    if (!mqtt_client.connected()) set_up_network();
    if (millis() - lastMillis > 1000) {
      lastMillis = millis();
      mqtt_client.publish("teamE/test/mkr1000-heartbeat", msg_str);
    } 
  }
}
