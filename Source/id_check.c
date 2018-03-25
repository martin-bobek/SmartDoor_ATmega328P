#include "id_check.h"
#include "mfrc.h"

#define RFID_TIMEOUT		1000
#define DOORID				0x880434C7
#define PETID				0x880434C7

uint8_t G_RfidDetected;

void IdCheckThread(void) {
	static uint16_t petTimer, mainTimer;

	if (G_PiccUid[DOOR_SPI] == DOORID) {
		G_RfidDetected |= MAIN_DOOR_RFID;
		mainTimer = RFID_TIMEOUT;
		G_PiccUid[DOOR_SPI] = 0;
	}
	if (G_PiccUid[PET_SPI] == PETID) {
		G_RfidDetected |= PET_DOOR_RFID;
		petTimer = RFID_TIMEOUT;
		G_PiccUid[PET_SPI] = 0;
	}

	if (petTimer == 1)
		G_RfidDetected &= ~PET_DOOR_RFID;
	if (petTimer != 0)
		petTimer--;

	if (mainTimer == 1)
		G_RfidDetected &= ~MAIN_DOOR_RFID;
	if (mainTimer != 0)
		mainTimer--;
}
