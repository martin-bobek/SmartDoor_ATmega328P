#include "id_check.h"
#include "mfrc.h"

#define RFID_TIMEOUT		1000
#define NUM_ID				4
#define PET_BASE_ADDR		4
#define DOOR_BASE_ADDR		21

static uint8_t numPet, prevNumPet;
static uint32_t petIds[4];
static uint8_t numDoor, prevNumDoor;
static uint32_t doorIds[4];

uint8_t G_AddId;
uint8_t G_RfidDetected;

void InitId(void) {
	uint8_t *dataPtr = (uint8_t *)petIds;

	EEARL = PET_BASE_ADDR;
	EECR = MSK(EERE);
	numPet = ~EEDR;
	prevNumPet = numPet;

	for (uint8_t i = 1; i <= numPet * 4; i++, dataPtr++) {
		EEARL = PET_BASE_ADDR + i;
		EECR = MSK(EERE);
		*dataPtr = EEDR;
	}

	dataPtr = (uint8_t *)doorIds;

	EEARL = DOOR_BASE_ADDR;
	EECR = MSK(EERE);
	numDoor = ~EEDR;
	prevNumDoor = numDoor;

	for (uint8_t i = 1; i <= numDoor * 4; i++, dataPtr++) {
		EEARL = DOOR_BASE_ADDR + i;
		EECR = MSK(EERE);
		*dataPtr = EEDR;
	}
}

void IdLogService(void) {
	static uint8_t state;
	static uint8_t *dataPtr;
	static uint8_t startAddr, endAddr;

	switch (state) {
	case 1:
		if (!(EECR & MSK(EEPE))) {
			EEARL = PET_BASE_ADDR;
			EEDR = ~numPet;
			EECR = MSK(EEMPE);
			EECR = MSK(EEPE);
			if (startAddr == 0)
				state = 0;
			else
				state = 3;
		}
		return;
	case 2:
		if (!(EECR & MSK(EEPE))) {
			EEARL = DOOR_BASE_ADDR;
			EEDR = ~numDoor;
			EECR = MSK(EEMPE);
			EECR = MSK(EEPE);
			if (startAddr == 0)
				state = 0;
			else
				state = 3;
		}
		return;
	case 3:
		if (!(EECR & MSK(EEPE))) {
			EEARL = startAddr;
			EEDR = *dataPtr;
			EECR = MSK(EEMPE);
			EECR = MSK(EEPE);
			startAddr++;
			dataPtr++;
			if (startAddr >= endAddr)
				state = 0;
		}
		return;
	}

	if (numPet != prevNumPet) {
		if (numPet == 0)
			startAddr = 0;
		else {
			startAddr = PET_BASE_ADDR + 4 * prevNumPet + 1;
			endAddr = PET_BASE_ADDR + 4 * numPet + 1;
			dataPtr = (uint8_t *)petIds + 4 * prevNumPet;
		}
		prevNumPet = numPet;
		state = 1;
	}
	else if (numDoor != prevNumDoor) {
		if (numDoor == 0)
			startAddr = 0;
		else {
			startAddr = DOOR_BASE_ADDR + 4 * prevNumDoor + 1;
			endAddr = DOOR_BASE_ADDR + 4 * numDoor + 1;
			dataPtr = (uint8_t *)doorIds + 4 * prevNumDoor;
		}
		prevNumDoor = numDoor;
		state = 2;
	}
}

void IdCheckThread(void) {
	static uint16_t petTimer, mainTimer;

	if (G_AddId & PET_DOOR_DELETE) {
		G_AddId = 0;
		numPet = 0;
		return;
	}
	if (G_AddId & MAIN_DOOR_DELETE) {
		G_AddId = 0;
		numDoor = 0;
		return;
	}

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

	if (G_AddId & PET_DOOR_RFID) {
		if (G_PiccUid[DOOR_SPI] != 0) {
			uint8_t detected = 0;
			for (uint8_t i = 0; i < numPet; i++) {
				if (G_PiccUid[DOOR_SPI] == petIds[i]) {
					detected = 1;
					break;
				}
			}
			if (!detected && numPet < NUM_ID) {
				petIds[numPet] = G_PiccUid[DOOR_SPI];
				numPet++;
			}
			G_AddId = 0;
			G_PiccUid[DOOR_SPI] = 0;
		}
		return;
	}

	if (G_PiccUid[DOOR_SPI] != 0) {
		uint8_t detected = 0;
		for (uint8_t i = 0; i < numDoor; i++) {
			if (G_PiccUid[DOOR_SPI] == doorIds[i]) {
				detected = 1;
				break;
			}
		}
		if (detected) {
			if (G_AddId & MAIN_DOOR_RFID) {
				G_AddId = 0;
			}
			else {
				G_RfidDetected |= MAIN_DOOR_RFID;
				mainTimer = RFID_TIMEOUT;
			}
		}
		else if (G_AddId & MAIN_DOOR_RFID) {
			if (numDoor < NUM_ID) {
				doorIds[numDoor] = G_PiccUid[DOOR_SPI];
				numDoor++;
			}
			G_AddId = 0;
		}
		G_PiccUid[DOOR_SPI] = 0;
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
