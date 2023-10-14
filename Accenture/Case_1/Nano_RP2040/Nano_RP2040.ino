#include <Arduino_LSM6DSOX.h>
unsigned long delay_ms, delay_sum_ms;
int max_t, min_t, sum, n_temperature;
enum state_enum {NORMAL, ALERT, FAULT};
state_enum state; 
#define MAX_TMP 30
#define TA_MS 60000
void setup() {
  // put your setup code here, to run once:
if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  
  }
  delay_ms = 10000;
  reset_temperature();
  state = NORMAL;
}

void loop() {
  switch (state) {
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
      Serial.print("Recap last minute: MAX = ");
      Serial.print(max_t);
      Serial.print(" °C");
      Serial.print(", MIN = ");
      Serial.print(min_t);
      Serial.print(" °C");
      Serial.print(", AVG = ");
      Serial.print(avg);
      Serial.println(" °C");
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