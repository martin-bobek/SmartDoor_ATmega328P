#ifndef TIMES_H_
#define TIMES_H_

#include "util.h"

#define WRITE_UNLOCK		MSK(0)
#define WRITE_LOCK			MSK(1)

extern uint8_t G_WriteTime;
extern uint8_t G_OpenTime;

extern uint8_t G_UnlockHour;
extern uint8_t G_UnlockMinute;
extern uint8_t G_UnlockSecond;

extern uint8_t G_LockHour;
extern uint8_t G_LockMinute;
extern uint8_t G_LockSecond;

void InitTimes(void);
void TimesThread(void);

#endif
