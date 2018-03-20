#include "lock.h"
#include "servo.h"

uint8_t G_LockPosition;

static uint8_t currentPosition;
static servo_t currentService;
static uint8_t servicing;

static void adjust(void);

void LockThread(void) {
	if (servicing)
		return adjust();

	if (currentPosition != G_LockPosition) {
		uint8_t difference = currentPosition ^ G_LockPosition;
		while (!(difference & MSK(currentService))) {
			currentService++;
			if (currentService >= 4)
				currentService = 0;
		}
		servicing = 1;
	}
}

static void adjust(void) {
	static uint8_t timer;
	static uint8_t positions[4];

	timer++;
	if (timer == 20) {
		timer = 0;
		if (G_LockPosition & MSK(currentService)) {	// moving to larger values
			if (positions[currentService] == MAX_POS) {
				currentPosition |= MSK(currentService);
				servicing = 0;
				return;
			}
			positions[currentService]++;
		}
		else {	// moving to smaller values
			if (positions[currentService] == 0) {
				currentPosition &= ~MSK(currentService);
				servicing = 0;
				return;
			}
			positions[currentService]--;
		}
		ServoPosition(currentService, positions[currentService]);
	}
}
