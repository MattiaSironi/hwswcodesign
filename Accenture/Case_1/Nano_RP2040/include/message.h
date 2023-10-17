#pragma once
#include <stdint.h>

typedef struct {
  uint8_t   id;
  uint8_t   status;
  int       max_temp;
  int       min_temp;
  float     avg_temp;
} message_t;
