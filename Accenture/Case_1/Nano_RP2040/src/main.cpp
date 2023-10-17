#include <Arduino.h>

#include <Arduino_LSM6DSOX.h>

#include "message.h"

#define MAX_TMP 30
#define TA_MS 1000

// #define INTERACTIVE

#if defined(INTERACTIVE)
#define DEBUG(x) { Serial.print(x); }
#else
#define DEBUG(x) {}
#undef Serial
#define Serial UART(p0, p1)
#endif

message_t message_to_send;

unsigned long delay_ms, delay_sum_ms;
int max_t, min_t, sum, n_temperature;

void reset_temperature();
void normal();
void alert();
void fault();

enum state_enum
{
  NORMAL,
  ALERT,
  FAULT,
};
state_enum state;

void setup()
{
  Serial.begin(9600);
  if (!IMU.begin())
  {
    DEBUG("Failed to initialize IMU!\n");
    while (1)
      ;
  }

  delay_ms = 100;

  reset_temperature();
  state = NORMAL;
}

void loop()
{
  switch (state)
  {
  case NORMAL:
    normal();
    break;
  case ALERT:
    alert();
    break;
  default:
    fault();
    break;
  }
}

void normal()
{
  // put your main code here, to run repeatedly:
  delay(delay_ms);
  if (IMU.temperatureAvailable())
  {
    int temperature_deg = 0;
    IMU.readTemperature(temperature_deg);

    DEBUG("LSM6DSOX Temperature = ");
    DEBUG(temperature_deg);
    DEBUG(" °C\n");
    if (temperature_deg <= MAX_TMP)
    {
      n_temperature++;
      sum += temperature_deg;
      if (temperature_deg > max_t)
        max_t = temperature_deg;
      if (temperature_deg < min_t)
        min_t = temperature_deg;
      delay_sum_ms += delay_ms;
      if (delay_sum_ms >= TA_MS)
      {
        float avg = (float)sum / (float)n_temperature;

        message_to_send = {
            1,
            (uint8_t)NORMAL,
            true,
            max_t,
            min_t,
            avg,
        };

        DEBUG("Recap last minute: MAX = ");
        DEBUG(message_to_send.max_temp);
        DEBUG(" °C");
        DEBUG(", MIN = ");
        DEBUG(message_to_send.min_temp);
        DEBUG(" °C");
        DEBUG(", AVG = ");
        DEBUG(message_to_send.avg_temp);
        DEBUG(" °C\n");

        #if not defined(INTERACTIVE)
        Serial.write((uint8_t *)&message_to_send, sizeof(message_t));
        #endif

        reset_temperature();
      }
    }
    else
    {
      state = ALERT;
    }
  }
}

void reset_temperature()
{
  max_t = 0;
  min_t = 200;
  sum = 0;
  n_temperature = 0;
  delay_sum_ms = 0;
}

void alert()
{
  DEBUG("ALERT");
  delay(500);
}

void fault()
{
  DEBUG("FAULT");
  delay(500);
}
