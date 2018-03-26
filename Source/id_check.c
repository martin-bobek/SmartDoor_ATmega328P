#include "id_check.h"
#include "mfrc.h"

#define RFID_TIMEOUT		1000
#define NUM_ID				4

static uint8_t numPet = 1;
static uint32_t petIds[4] = { 0x880434C7 };
static uint8_t numDoor = 1;
static uint32_t doorIds[4] = { 0x880434C7 };

uint8_t G_AddId;
uint8_t G_RfidDetected;

void IdCheckThread(void) {
	static uint16_t petTimer, mainTimer;

	if (G_PiccUid[PET_SPI] != 0) {
		uint8_t detected = 0;
		for (uint8_t i = 0; i < numPet; i++) {
			if (G_PiccUid[PET_SPI] == petIds[i]) {
				detected = 1;
				break;
			}
		}
		G_PiccUid[PET_SPI] = 0;
		if (detected) {
			G_RfidDetected |= PET_DOOR_RFID;
			petTimer = RFID_TIMEOUT;
		}
	}

	if (G_PiccUid[DOOR_SPI] != 0) {
		uint8_t detected = 0;
		for (uint8_t i = 0; i < numDoor; i++) {
			if (G_PiccUid[DOOR_SPI] == doorIds[i]) {
				detected = 1;
				break;
			}
		}
		G_PiccUid[DOOR_SPI] = 0;
		if (detected) {
			G_RfidDetected |= MAIN_DOOR_RFID;
			mainTimer = RFID_TIMEOUT;
		}
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
