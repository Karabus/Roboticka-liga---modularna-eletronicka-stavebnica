#ifndef GLOBALS_H
#define GLOBALS_H

#include "maze_map.h"
#include <stdint.h>

extern Map          mapa;
extern Cell*        root;
extern uint8_t      curDirection;
extern uint8_t      curX;
extern uint8_t      curY;
extern const int8_t mapDirection[4][2];

#endif