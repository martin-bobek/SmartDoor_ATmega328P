#include "buttons.h"
#include "twi.h"

uint8_t G_ButtonPressed;

void ButtonService(void) {
	static uint8_t released;
	G_ButtonPressed |= 0xF & G_TinyStatus & released;
	released = ~G_TinyStatus;
}
