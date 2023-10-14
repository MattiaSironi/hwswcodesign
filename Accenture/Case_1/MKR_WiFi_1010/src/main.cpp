#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>

static const char *ssid = WIFI_SSID;
static const char *wifi_password = WIFI_PASSWORD;
static const char *host = MQTT_HOST;
static const int port = MQTT_PORT;
static const char *username = MQTT_USERNAME;
static const char *mqtt_password = MQTT_PASSWORD;

WiFiClient wifi_client;
MQTTClient mqtt_client;

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
  SerialUSB.begin(115200);
  while (!SerialUSB.available());
  Serial1.begin(115200);
  while (!SerialUSB.available());
  set_up_network();
  mqtt_client.subscribe("/teamE/test/receive");
}

void loop() {
  mqtt_client.loop();
  if (!mqtt_client.connected()) set_up_network();

  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    mqtt_client.publish("/teamE/test/mkr1000-heartbeat", "");
  }
}
