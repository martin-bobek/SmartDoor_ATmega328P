#include "main_door.h"
#include "id_check.h"
#include "lock.h"

#define TIMEOUT			5000

uint8_t G_UnlockRequest;

void MainDoorThread(void) {
	static uint16_t timer;

	if (G_UnlockRequest || (G_RfidDetected & MAIN_DOOR_RFID)) {
		G_UnlockRequest = 0;
		timer = TIMEOUT;
		G_LockPosition |= DOOR_LOCK;
	}

	if (timer == 1)
			G_LockPosition &= ~DOOR_LOCK;
	if (timer > 0)
		timer--;
}
