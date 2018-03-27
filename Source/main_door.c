#include "main_door.h"
#include "id_check.h"
#include "lock.h"
#include "hall.h"

#define TIMEOUT			5000

uint8_t G_UnlockRequest;

void MainDoorThread(void) {
	static uint16_t timer;

	if (G_LockPosition & DOOR_LOCK) {
		if (G_UnlockRequest || (G_RfidDetected & MAIN_DOOR_RFID) || !(G_DoorClosed & DOOR_CLOSED)) {
			G_RfidDetected &= ~MAIN_DOOR_RFID;
			timer = 0;
		}
		else {
			timer++;
			if (timer >= TIMEOUT) {
				G_LockPosition &= ~DOOR_LOCK;
				UNLOCKED_OFF();
				timer = 0;
			}
		}
	}
	else if (G_UnlockRequest || (G_RfidDetected & MAIN_DOOR_RFID)) {
		UNLOCKED_ON();
		G_LockPosition |= DOOR_LOCK;
		G_RfidDetected &= ~MAIN_DOOR_RFID;
	}

	G_UnlockRequest = 0;
}
