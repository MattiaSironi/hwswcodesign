#include <Arduino.h>

#include <Arduino_LSM6DSOX.h>

#include "message.h"

#define MAX_TMP 30
#define TA_MS 1000

#define UART_BIT_PERIOD_US 104 // approximately 1/9600
#define RX_PIN p15
#define TX_PIN p25

#define DEBUG(x) Serial.print(x)

message_t message_to_send;

unsigned long delay_ms, delay_sum_ms;
int max_t, min_t, sum, n_temperature;

void software_uart_transmit(uint8_t *buffer, size_t number_bytes);
void software_uart_receive();

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

void software_uart_receive() {}

void software_uart_write(PinStatus value) {
  digitalWrite(TX_PIN, value);
  delayMicroseconds(104);
}

uint8_t get_bit(uint8_t vector, size_t index) {
  return (vector & 1 >> index) << index;
}

void software_uart_transmit(uint8_t *buffer, size_t number_bytes) {
  // Default Arduino configuration is 8N1, meaning
  // - 8 data bits
  // - No parity bit
  // - 1 stop bit

  // Data
  for (; number_bytes > 0; number_bytes--) {
    // Start bit
    software_uart_write(LOW);
    for(int i = 0; i < 8; i++) {
      software_uart_write((PinStatus) get_bit(*buffer, i));
    }
    // Stop bit
    software_uart_write(HIGH);
    // Keep the line high later
    software_uart_write(HIGH);

    buffer++;
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  pinMode(TX_PIN, OUTPUT);

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

        uint8_t *message_pointer = (uint8_t *)&message_to_send;
        software_uart_transmit(message_pointer, sizeof(message_t));

        reset_temperature();
      }
    }
    else
    {
      // state = ALERT;
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
