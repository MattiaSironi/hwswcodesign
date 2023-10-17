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


// "id,status,max,min,avg" id = index 0, status index 2, max index 4-5, min index 7-8, avg index 10-11-12-13-14;
