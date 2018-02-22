#include "pet_door.h"
#include "lock.h"

#define INSIDE_MSK			(TINY_IR1 | TINY_IR2)
#define OUTSIDE_MSK			(TINY_IR3 | TINY_IR4)
#define CLOSE_DELAY			5000

typedef enum { CLOSED, IN_OPEN, OUT_OPEN } state_t;

void PetDoorThread(void) {
	static state_t state = CLOSED;
	static uint16_t timer;

	switch (state) {
	case CLOSED:
		if (G_TinyStatus & INSIDE_MSK) {
			G_LockPosition |= IN_PET_LOCK;
			state = IN_OPEN;
		}
		else if (G_TinyStatus & OUTSIDE_MSK) {
			G_LockPosition |= OUT_PET_LOCK;
			state = OUT_OPEN;
		}
		break;
	case IN_OPEN:
		if (G_TinyStatus & INSIDE_MSK)
			timer = 0;
		else {
			timer++;
			if (timer >= CLOSE_DELAY) {
				G_LockPosition &= ~IN_PET_LOCK;
				timer = 0;
				state = CLOSED;
			}
		}
		break;
	case OUT_OPEN:
		if (G_TinyStatus & OUTSIDE_MSK)
			timer = 0;
		else {
			timer++;
			if (timer >= CLOSE_DELAY) {
				G_LockPosition &= ~OUT_PET_LOCK;
				timer = 0;
				state = CLOSED;
			}
		}
		break;
	}
}