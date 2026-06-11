#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include "hardware.h"
#include "motion.h"
#include "maze_map.h"
#include "globals.h"

enum State { START, EXPLORE, IDLE, NAVIGATE };
State getState();
void  explore();
void  mouseInit();
static bool readGoalFromSerial(uint8_t& gx, uint8_t& gy);
void  mouseTick();
State getState();
int   goToPos(uint8_t nextX, uint8_t nextY);

#endif