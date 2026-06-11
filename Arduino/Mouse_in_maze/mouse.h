#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include "globals.h"
#include "hardware.h"
#include "motion.h"
#include "maze_map.h"

enum State { START, EXPLORE, IDLE, NAVIGATE };
State getState();
void  explore();
void  mouseInit();
void  mouseTick();
int   goToPos(uint8_t nextX, uint8_t nextY);

#endif