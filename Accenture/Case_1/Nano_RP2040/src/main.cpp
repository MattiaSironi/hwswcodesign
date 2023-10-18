#include <Arduino_LSM6DSOX.h>
#include "message.h"

#define LINE_ID 0
#define MAX_TMP 40
#define TA_MS 60000

void reset_temperature();
void normal();
void alert();
void fault();
void debug();

unsigned long delay_ms, delay_sum_ms;
int max_t, min_t, avg_t, sum, n_temperature;
enum state_enum {NORMAL, ALERT, FAULT, DEBUG};
state_enum state;


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
    case DEBUG : DEBUG(); break;
    case NORMAL: normal(); break;
    case ALERT: alert(); break;
    default: fault(); break;
  }
}

void normal() {
  // put your main code here, to run repeatedly:
  delay_ms = 10000;
  delay(delay_ms);
  retrieve_temperature();
  // "id,status,max,min,avg" id = index 0, status index 2, max index 4-5, min index 7-8, avg index 10-11-12-13-14;
  send_message(state);
  reset_temperature();
    }
  
void retrieve_temperature() {
  if (IMU.temperatureAvailable())
  {
    int temperature_deg = 0;
    IMU.readTemperature(temperature_deg);

    Serial.print("LSM6DSOX Temperature = ");
    Serial.print(temperature_deg);
    Serial.println(" °C");

    if (temperature_deg > MAX_TMP) {
      state = ALERT;
    
    }
    else {
      state = NORMAL;

    } 
    n_temperature++;
    sum+=temperature_deg;
    if (temperature_deg > max_t) max_t = temperature_deg;
    if (temperature_deg < min_t) min_t = temperature_deg;
    delay_sum_ms+=delay_ms;
    if ( delay_sum_ms >= TA_MS) {
      int avg_t = sum / n_temperature;
      Serial.print("Recap last minute: MAX = ");
      Serial.print(max_t);
      Serial.print(" °C");
      Serial.print(", MIN = ");
      Serial.print(min_t);
      Serial.print(" °C");
      Serial.print(", AVG = ");
      Serial.print(avg_t);
      Serial.println(" °C");
    }
} 
}

void send_message(state_enum state) {
  char buffer[15];
  switch (state) {
    case NORMAL : sprintf(buffer, "%d,%d,%d,%d,%.2f", LINE_ID, state, max_t, min_t, avg_t); break;
    case ALERT: case FAULT : sprintf(buffer, "%d,%d,0,0,0.00", LINE_ID, state); break;
    default : return;
}
  Serial1.write(buffer);
}

void reset_temperature() {
  max_t = 0;
  min_t = 200;
  sum = 0;
  n_temperature= 0;
  delay_sum_ms = 0;
}

void alert() {
  delay_ms = 1000;
  delay(delay_ms);
  retrieve_temperature();
  send_message(state);
  reset_temperature
  Serial.println("ALERT");
  
}

void fault() {
  send_message(state);
  while (1) ; //line shut_down
}
void debug() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
