/**
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "peripherals.h"
#include "reg_map.h"

#include "activity_classifier.h"

// sensitivity values taken from the datasheet for the selected full-scale
#define ACC_SENS_8G_MG 0.244f
#define GYRO_SENS_2000DPS 0.070f
#define SENSOR_NUM_AXES 6
#define SENSOR_ODR 104
#define WIN_LEN_IN_SEC 1
#define WIN_LEN_IN_SAMPLES (SENSOR_ODR * WIN_LEN_IN_SEC)

void __attribute__((signal)) algo_00_init(void);
void __attribute__((signal)) algo_00(void);
// user defined function
void reset_status(void);

static volatile uint32_t int_status;
static float mean[SENSOR_NUM_AXES];
static uint8_t win_count;

void __attribute__((signal)) algo_00_init(void) { reset_status(); }

// reset window counter and accumulators
inline void reset_status(void) {
  for (uint8_t ax = 0; ax < SENSOR_NUM_AXES; ax++) {
    mean[ax] = 0.0f;
  }
  win_count = 0;
}

void __attribute__((signal)) algo_00(void) {
  size_t in_addr = ISPU_ARAW_X;
  win_count++;
  // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
  for (uint8_t ax = 0; ax < SENSOR_NUM_AXES; ax++) {
    float sens = ax < 3 ? ACC_SENS_8G_MG : GYRO_SENS_2000DPS;
    // update the mean value
    float measurement = (float)cast_sint16_t(in_addr) * sens;
    mean[ax] += (measurement - mean[ax]) / win_count;
    in_addr += 4;
  }

  // if the selected number of samples has already been collected
  if (win_count == WIN_LEN_IN_SAMPLES) {
    size_t out_addr = ISPU_DOUT_00;
    uint8_t activity_class = activity_classifier_predict(mean);
    // output the mean
    for (uint8_t ax = 0; ax < SENSOR_NUM_AXES; ax++) {
      cast_float(out_addr) = mean[ax];
      out_addr += 4;
    }
    cast_uint8_t(out_addr) = activity_class;

    reset_status();
  }

  // interrupt generation (set bit 0 for algo 0, bit 1 for algo 1, etc.)
  int_status = int_status | 0x1u;
}

// For more algorithms implement the corresponding functions: algo_01_init and
// algo_01 for algo 1, algo_02_init and algo_02 for algo 2, etc.

int main(void) {
  // set boot done flag
  uint8_t status = cast_uint8_t(ISPU_STATUS);
  status = status | 0x04u;
  cast_uint8_t(ISPU_STATUS) = status;

  // enable algorithms interrupt request generation
  cast_uint8_t(ISPU_GLB_CALL_EN) = 0x01u;

  while (true) {
    stop_and_wait_start_pulse;

    // reset status registers and interrupts
    int_status = 0u;
    cast_uint32_t(ISPU_INT_STATUS) = 0u;
    cast_uint8_t(ISPU_INT_PIN) = 0u;

    // get all the algorithms to run in this time slot
    cast_uint32_t(ISPU_CALL_EN) = cast_uint32_t(ISPU_ALGO) << 1;

    // wait for all algorithms execution
    while (cast_uint32_t(ISPU_CALL_EN) != 0u) {
    }

    // get interrupt flags
    uint8_t int_pin = 0u;
    int_pin |=
        ((int_status & cast_uint32_t(ISPU_INT1_CTRL)) > 0u) ? 0x01u : 0x00u;
    int_pin |=
        ((int_status & cast_uint32_t(ISPU_INT2_CTRL)) > 0u) ? 0x02u : 0x00u;

    // set status registers and generate interrupts
    cast_uint32_t(ISPU_INT_STATUS) = int_status;
    cast_uint8_t(ISPU_INT_PIN) = int_pin;
  }
}
