#include <Arduino.h>

#include <WiFi.h>
#include <MQTT.h>

#include "message.h"

static uint8_t heartbeat_counter;
static size_t number_bytes_read;

uint8_t received_message_buffer[sizeof(message_t)];
message_t *received_message;

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

void set_up_network()
{
  // FIXME Remove when MQTT broker is back up
  return;

  SerialUSB.println("Setting up network connection");

  WiFi.begin(ssid, wifi_password);
  SerialUSB.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    SerialUSB.print(".");
    delay(1000);
  }
  SerialUSB.println("\nWiFi connection successful!");

  mqtt_client.begin(host, port, wifi_client);
  mqtt_client.onMessage(messageCallback);
  SerialUSB.println("Connecting to MQTT broker");
  while (!mqtt_client.connect("mkr1000", username, mqtt_password))
  {
    SerialUSB.print(".");
    delay(500);
  }
  SerialUSB.println("\nMQTT connection successful!");
}

void messageCallback(String &topic, String &payload)
{
  SerialUSB.println("Received message '" + payload + "' from topic " + topic);
}

void setup()
{
  SerialUSB.begin(9600);
  while (!SerialUSB)
    ;
  Serial1.begin(9600);
  while (!Serial1)
    ;
  set_up_network();
  mqtt_client.subscribe("teamE/test/receive");
}

void loop()
{
  // mqtt_client.loop();
  if (!mqtt_client.connected())
    set_up_network();
  if (millis() - lastMillis > 1000)
  {
    heartbeat_counter += 1;
    SerialUSB.println(heartbeat_counter);
    lastMillis = millis();
    // mqtt_client.publish("teamE/test/mkr1000-heartbeat", String(heartbeat_counter));
  }

  if (Serial1.available())
  {
    number_bytes_read = Serial1.readBytes(received_message_buffer, sizeof(message_t));
    if (number_bytes_read > 0)
    {
      received_message = (message_t *)received_message_buffer;
      SerialUSB.print("Received MAX temp is ");
      SerialUSB.println(received_message->max_temp);
    }
    else
    {
      SerialUSB.println("No data to read");
    }
  }
}
