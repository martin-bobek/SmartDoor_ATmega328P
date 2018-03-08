#ifndef TWI_H__
#define TWI_H__

#include "util.h"

#define TWI_ON                  (MSK(TWEN) | MSK(TWIE))
#define LCD_ADDR                (uint8_t)0x4E
#define TINY_ADDR				(uint8_t)0xA5

typedef enum { TWI_NOERROR, TWI_STARTFAIL, TWI_ADDRESSFAIL, TWI_DATAFAIL } twi_error_t;
extern volatile twi_error_t G_TwiError;

uint8_t TwiSend(uint8_t address, uint8_t data);
uint8_t TwiRecieve(uint8_t address);
void TwiService(void);

#endif
