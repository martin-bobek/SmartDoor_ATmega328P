#include "times.h"
#include "rtc.h"

typedef enum { IDLE, UNLOCK_HOUR, UNLOCK_MINUTE, LOCK_HOUR, LOCK_MINUTE } writeState_t;

uint8_t G_WriteTime;
uint8_t G_OpenTime;

uint8_t G_UnlockHour;
uint8_t G_UnlockMinute;

uint8_t G_LockHour;
uint8_t G_LockMinute;

void InitTimes(void) {
	EEAR = 0;
	EECR = MSK(EERE);
	G_UnlockHour = EEDR;

	EEARL = 1;
	EECR = MSK(EERE);
	G_UnlockMinute = EEDR;

	EEARL = 2;
	EECR = MSK(EERE);
	G_LockHour = EEDR;

	EEARL = 3;
	EECR = MSK(EERE);
	G_LockMinute = EEDR;
}

void TimesThread(void) {
	static writeState_t state = IDLE;

	switch (state) {
	case IDLE:
		if (G_WriteTime & WRITE_UNLOCK)
			state = UNLOCK_HOUR;
		else if (G_WriteTime & WRITE_LOCK)
			state = LOCK_HOUR;
		break;
	case UNLOCK_HOUR:
		if (!(EECR & MSK(EEPE))) {
			EEARL = 0;
			EEDR = G_UnlockHour;
			EECR = MSK(EEMPE);
			EECR = MSK(EEPE);
			state = UNLOCK_MINUTE;
		}
		break;
	case UNLOCK_MINUTE:
		if (!(EECR & MSK(EEPE))) {
			EEARL = 1;
			EEDR = G_UnlockMinute;
			EECR = MSK(EEMPE);
			EECR = MSK(EEPE);
			G_WriteTime &= ~WRITE_UNLOCK;
			state = IDLE;
		}
		break;
	case LOCK_HOUR:
		if (!(EECR & MSK(EEPE))) {
			EEARL = 2;
			EEDR = G_LockHour;
			EECR = MSK(EEMPE);
			EECR = MSK(EEPE);
			state = LOCK_MINUTE;
		}
		break;
	case LOCK_MINUTE:
		if (!(EECR & MSK(EEPE))) {
			EEARL = 3;
			EEDR = G_LockMinute;
			EECR = MSK(EEMPE);
			EECR = MSK(EEPE);
			G_WriteTime &= ~WRITE_LOCK;
			state = IDLE;
		}
		break;
	}

	if ((G_Hours < G_UnlockHour) ||
			(G_Hours == G_UnlockHour && G_Minutes < G_UnlockMinute) ||
			(G_Hours > G_LockHour) ||
			(G_Hours == G_LockHour && G_Minutes >= G_LockMinute)) {
		G_OpenTime = 0;
		return;
	}
	G_OpenTime = 1;
}
