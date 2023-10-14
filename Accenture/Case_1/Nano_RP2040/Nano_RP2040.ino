#include <Arduino_LSM6DSOX.h>
unsigned long delay_ms, delay_sum_ms, ta_ms;
int max_t, min_t, sum, n_temperature;
void setup() {
  // put your setup code here, to run once:
if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  
  }
  delay_ms = 10000;
  ta_ms = 60000;
  reset_temperature(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(delay_ms);
  if (IMU.temperatureAvailable())
  {
    int temperature_deg = 0;
    IMU.readTemperature(temperature_deg);

    Serial.print("LSM6DSOX Temperature = ");
    Serial.print(temperature_deg);
    Serial.println(" °C");
    n_temperature++;
    sum+=temperature_deg;
    if (temperature_deg > max_t) max_t = temperature_deg;
    if (temperature_deg < min_t) min_t = temperature_deg;
    delay_sum_ms+=delay_ms;
    if (  delay_sum_ms >= ta_ms) {
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

}

void reset_temperature(){
  max_t = 0;
  min_t = 200;
  sum = 0;
  n_temperature= 0;
  delay_sum_ms = 0;
}
