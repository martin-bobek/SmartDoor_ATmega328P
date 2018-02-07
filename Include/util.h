#ifndef UTIL_H__
#define UTIL_H__

#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct {
	uint8_t Bit0 : 1;
	uint8_t Bit1 : 1;
	uint8_t Bit2 : 1;
	uint8_t Bit3 : 1;
	uint8_t Bit4 : 1;
	uint8_t Bit5 : 1;
	uint8_t Bit6 : 1;
	uint8_t Bit7 : 1;
} BITFIELD;

#define REG(ADDR)		(*(volatile BITFIELD *)&ADDR)
#define MSK(BIT)        (uint8_t)(1 << (BIT))

#endif
