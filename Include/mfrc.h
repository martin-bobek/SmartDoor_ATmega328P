#ifndef MFRC_H_
#define MFRC_H_

#include "util.h"

extern uint8_t G_MfrcTestFlag;
extern uint32_t G_PiccUid;

extern void (*MfrcService)(void);

#endif