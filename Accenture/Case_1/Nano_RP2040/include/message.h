#pragma once
#include <stdint.h>

typedef struct {
  uint8_t   id;
  uint8_t   status;
  bool      do_continue_reading;
  int       max_temp;
  int       min_temp;
  float     avg_temp;
} message_t;
