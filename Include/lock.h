#ifndef LOCK_H_
#define LOCK_H_

#include "util.h"

#define IN_PET_LOCK		MSK(0)
#define OUT_PET_LOCK	MSK(1)
#define DOOR_LOCK		MSK(2)
#define MAIL_LOCK		MSK(3)

extern uint8_t G_LockPosition;

void LockThread(void);

#endif
