#include "mailbox.h"
#include "lock.h"
#include "hall.h"
#include "buttons.h"

typedef enum { INIT, UNLOCKED_CLOSED, UNLOCKED_OPEN, LOCKED } state_t;

#define WAIT_TIME		1000

void MailboxThread(void) {
	static uint16_t timer = 100;
	static state_t state = INIT;

	switch (state) {
	case INIT:
		timer--;
		if (timer == 0) {
			if (G_DoorClosed & MAIL_CLOSED)
				state = LOCKED;
			else {
				state = UNLOCKED_OPEN;
				G_LockPosition |= MAIL_LOCK;
			}
		}
		break;
	case UNLOCKED_CLOSED:
		if (!(G_DoorClosed & MAIL_CLOSED))
			state = UNLOCKED_OPEN;
		break;
	case UNLOCKED_OPEN:
		if (G_DoorClosed & MAIL_CLOSED) {
			timer++;
			if (timer >= WAIT_TIME) {
				G_LockPosition &= ~MAIL_LOCK;
				state = LOCKED;
				timer = 0;
			}
		}
		else
			timer = 0;
		break;
	case LOCKED:
		if ((G_TinyStatus & MAIL_BUTTON) && (G_LockPosition & DOOR_LOCK)) {
			G_LockPosition |= MAIL_LOCK;
			state = UNLOCKED_CLOSED;
		}
		break;
	}
}
