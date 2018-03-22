#ifndef ID_CHECK_H_
#define ID_CHECK_H_

#include "util.h"

#define PET_DOOR_RFID		MSK(0)
#define MAIN_DOOR_RFID		MSK(1)

extern uint8_t G_RfidDetected;

void IdCheckThread(void);

#endif
