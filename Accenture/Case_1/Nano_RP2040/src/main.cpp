#include <Arduino_LSM6DSOX.h>

#include "message.h"

message_t message_to_send;

void reset_temperature();
void normal();
void alert();
void fault();
void debug();
unsigned long delay_ms, delay_sum_ms;
int max_t, min_t, sum, n_temperature;

enum state_enum {NORMAL, ALERT, FAULT, DEBUG};
state_enum state; 

#define MAX_TMP 30
#define TA_MS 60000

void setup() {
  Serial1.begin(9600);
  // put your setup code here, to run once:
if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  
  }
  delay_ms = 10000;
  reset_temperature();
  state = NORMAL;
  if (state == DEBUG) pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  switch (state) {
    case DEBUG : debug(); break;
    case NORMAL: normal(); break;
    case ALERT: alert(); break;
    default: fault(); break;
  }
}

void normal() {
  // put your main code here, to run repeatedly:
  delay(delay_ms);
  if (IMU.temperatureAvailable())
  {
    int temperature_deg = 0;
    IMU.readTemperature(temperature_deg);

    Serial.print("LSM6DSOX Temperature = ");
    Serial.print(temperature_deg);
    Serial.println(" °C");
    if (temperature_deg <= MAX_TMP) {
    n_temperature++;
    sum+=temperature_deg;
    if (temperature_deg > max_t) max_t = temperature_deg;
    if (temperature_deg < min_t) min_t = temperature_deg;
    delay_sum_ms+=delay_ms;
    if (  delay_sum_ms >= TA_MS) {
      float avg = (float)sum / (float)n_temperature;

      message_to_send = {
        1,
        (uint8_t)NORMAL,
        true, 
        max_t,
        min_t,
        avg,
      };

      Serial.print("Recap last minute: MAX = ");
      Serial.print(message_to_send.max_temp);
      Serial.print(" °C");
      Serial.print(", MIN = ");
      Serial.print(message_to_send.min_temp);
      Serial.print(" °C");
      Serial.print(", AVG = ");
      Serial.print(message_to_send.avg_temp);
      Serial.println(" °C");

      Serial.write((uint8_t *)&message_to_send, sizeof(message_t));

      reset_temperature();
    }
  }
  else {
    state = ALERT;
  }
  }

}

void reset_temperature(){
  max_t = 0;
  min_t = 200;
  sum = 0;
  n_temperature= 0;
  delay_sum_ms = 0;
}

void alert() {
  Serial.println("ALERT");
  delay(500);
}

void fault() {
  Serial.println("FAULT");
  delay(500);
}
void debug() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
