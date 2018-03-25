#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "util.h"

#define RIGHT_BUTTON		MSK(0)
#define MAIL_BUTTON			MSK(1)
#define LEFT_BUTTON			MSK(2)
#define MIDDLE_BUTTON		MSK(3)

extern uint8_t G_ButtonPressed;

void ButtonService(void);

#endif
