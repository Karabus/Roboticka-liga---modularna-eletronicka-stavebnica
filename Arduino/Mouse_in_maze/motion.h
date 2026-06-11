#ifndef MOTION_H
#define MOTION_H

#include <stdint.h>
#include "hardware.h"
#include "globals.h"

void stopMotors();
void goForward();
void turn(bool dir);

#endif