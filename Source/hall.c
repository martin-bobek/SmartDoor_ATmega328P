#include "hall.h"

#define THRESHOLD			0x90
#define FILTER_PERIOD		20

#define MAIN_MUX			MSK(MUX1)
#define PET_MUX				MSK(MUX0)
#define MAIL_MUX			(MSK(MUX1) | MSK(MUX0))

uint8_t G_DoorClosed;

uint8_t G_HallValue[3];
static uint8_t currentHall;

void AdcService(void) {
	const static uint8_t hallMux[3] = { MAIN_MUX, PET_MUX, MAIL_MUX };

	G_HallValue[currentHall] = ADCH;
	currentHall++;
	if (currentHall >= 3)
		currentHall = 0;
	ADMUX = ADMUX_SETUP | hallMux[currentHall];
	ADCSRA = ADCSRA_SETUP | MSK(ADSC);
}

void HallService(void) {
	static uint8_t timer[3];

	if ((G_HallValue[currentHall] > THRESHOLD) != ((G_DoorClosed & MSK(currentHall)) != 0)) {
		timer[currentHall]++;
		if (timer[currentHall] >= FILTER_PERIOD) {
			G_DoorClosed ^= MSK(currentHall);
			timer[currentHall] = 0;
		}
	}
	else {
		timer[currentHall] = 0;
	}
}
