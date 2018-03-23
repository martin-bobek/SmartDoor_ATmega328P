#ifndef MFRC_H_
#define MFRC_H_

#include "util.h"
#include "spi.h"

extern uint8_t G_MfrcTestFlag[NUM_RFID];
extern uint32_t G_PiccUid[NUM_RFID];

extern void (*MfrcService)(void);

#endif
