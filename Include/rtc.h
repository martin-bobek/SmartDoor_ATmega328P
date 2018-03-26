#ifndef RTC_H_
#define RTC_H_

#include "util.h"

#define CHANGE_FLAG             1
#define STOP_FLAG_SEC           MSK(7)
#define HOUR24_FLAG_HOUR        MSK(7)

extern uint8_t G_Seconds;
extern uint8_t G_Minutes;
extern uint8_t G_Hours;
extern uint8_t G_RtcWrite;

void RtcService(void);

#endif