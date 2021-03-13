#ifndef _UTILS_H
#define _UTILS_H

#include <Arduino.h>
#include "constants.h"

double getBatteryPercentage()
{
  static const double BATTERY_MODIFIER = 2 * (3.45 / 4095);
  double voltage = analogRead(BATTERY_PIN) * BATTERY_MODIFIER;
  double percentage = (voltage - 3.20) / (4.05 - 3.20);

  if (percentage < 0)
    return 0;
  if (percentage > 1)
    return 1;

  return percentage;
}

#endif
