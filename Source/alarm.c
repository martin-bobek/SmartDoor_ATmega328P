#include "alarm.h"
#include "lock.h"
#include "hall.h"
#include "id_check.h"

#define ALARM_DELAY			1000

void AlarmThread(void) {
	static uint16_t timer;

	if (ALARM_ISON()) {
		if (G_RfidDetected & MAIN_DOOR_RFID) {
			G_RfidDetected &= ~MAIN_DOOR_RFID;
			ALARM_OFF();
		}
	}
	else if ((!(G_LockPosition & DOOR_LOCK) && !(G_DoorClosed & DOOR_CLOSED)) ||
			(!(G_LockPosition & (IN_PET_LOCK | OUT_PET_LOCK)) && !(G_DoorClosed & PET_CLOSED)) ||
			(!(G_LockPosition & MAIL_LOCK) && !(G_DoorClosed & MAIL_CLOSED))) {
		timer++;
		if (timer >= ALARM_DELAY) {
			ALARM_ON();
			timer = 0;
		}
	}
	else
		timer = 0;
}
