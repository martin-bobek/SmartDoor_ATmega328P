#ifndef HALL_H_
#define HALL_H_

#include "util.h"

#define ADMUX_SETUP			MSK(ADLAR)
#define ADCSRA_SETUP		(MSK(ADEN) | MSK(ADPS2) | MSK(ADPS1))

#define DOOR_CLOSED			MSK(0)
#define MAIL_CLOSED			MSK(1)
#define PET_CLOSED			MSK(2)

extern uint8_t G_DoorClosed;
extern uint8_t G_HallValue[3];

void AdcService(void);
void HallService(void);

#endif
