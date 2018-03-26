#ifndef ID_CHECK_H_
#define ID_CHECK_H_

#include "util.h"

#define PET_DOOR_RFID		MSK(0)
#define MAIN_DOOR_RFID		MSK(1)
#define PET_DOOR_DELETE		MSK(2)
#define MAIN_DOOR_DELETE	MSK(3)

extern uint8_t G_RfidDetected;

extern uint8_t G_AddId;

void IdCheckThread(void);

#endif
