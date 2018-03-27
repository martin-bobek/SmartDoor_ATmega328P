#ifndef ID_CHECK_H_
#define ID_CHECK_H_

#include "util.h"

#define PET_DOOR_RFID		MSK(0)
#define MAIN_DOOR_RFID		MSK(1)
#define PET_DOOR_FULL		MSK(2)
#define MAIN_DOOR_FULL		MSK(3)

extern uint8_t G_RfidDetected;

#define PET_DOOR_DELETE		MSK(2)
#define MAIN_DOOR_DELETE	MSK(3)

extern uint8_t G_AddId;

void IdCheckThread(void);
void IdLogService(void);
void InitId(void);

#endif
