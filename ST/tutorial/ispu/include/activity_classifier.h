#pragma once

#include <stdint.h>

static inline int32_t
activity_classifier_predict_tree_0(const float *features) {
  if (features[2] < 337.0892333984375) {
    if (features[1] < 752.7792358398438) {
      if (features[1] < -619.2154235839844) {
        return 1;
      } else {
        if (features[1] < 558.8316955566406) {
          if (features[0] < -903.6760559082031) {
            return 3;
          } else {
            return 3;
          }
        } else {
          return 3;
        }
      }
    } else {
      if (features[0] < -198.67298889160156) {
        if (features[1] < 964.0451965332031) {
          if (features[0] < -688.8497314453125) {
            return 1;
          } else {
            return 1;
          }
        } else {
          return 1;
        }
      } else {
        if (features[0] < 68.24848175048828) {
          return 2;
        } else {
          if (features[3] < -0.5337255299091339) {
            return 2;
          } else {
            return 2;
          }
        }
      }
    }
  } else {
    if (features[0] < -67.42924499511719) {
      if (features[2] < 437.91233825683594) {
        if (features[0] < -886.8507690429688) {
          return 3;
        } else {
          if (features[5] < -0.637690544128418) {
            return 2;
          } else {
            return 2;
          }
        }
      } else {
        if (features[5] < -23.546794891357422) {
          return 3;
        } else {
          if (features[2] < 697.09912109375) {
            return 0;
          } else {
            return 0;
          }
        }
      }
    } else {
      if (features[3] < -0.9509706199169159) {
        return 2;
      } else {
        if (features[3] < 1.3026073575019836) {
          return 2;
        } else {
          return 2;
        }
      }
    }
  }
}

uint8_t activity_classifier_predict(const float *features) {
  return activity_classifier_predict_tree_0(features);
}
