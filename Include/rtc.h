#ifndef RTC_H_
#define RTC_H_

#include "util.h"

extern uint8_t G_Seconds;
extern uint8_t G_Minutes;
extern uint8_t G_Hours;

void RtcService(void);

#endif