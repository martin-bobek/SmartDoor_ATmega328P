#ifndef SERCAT
#define SERCAT

#include "stdbool.h"


bool ControlCatDoor(bool inDoorLED, bool outDoorLED, bool inDoorCat, bool outDoorCat, bool catDoorStatus);

#define INNEROPEN       1;
#define OUTEROPEN       2;
#define CLOSE           0;

#endif
