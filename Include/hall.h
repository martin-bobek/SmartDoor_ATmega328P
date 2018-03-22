#ifndef HALL_H_
#define HALL_H_

#include "util.h"

#define ADMUX_SETUP			MSK(ADLAR) // PLUS HALL EFFECT MUX BITS
#define ADCSRA_SETUP		(MSK(ADEN) | MSK(ADPS2) | MSK(ADPS1))// PLUS CLEAR INTERRUPT FLAG

#define MAIN_CLOSED			MSK(0)
#define PET_CLOSED			MSK(1)
#define MAIL_CLOSED			MSK(2)

extern uint8_t G_DoorClosed;

extern uint8_t G_HallValue[3];

void HallEffectService(void);

#endif
