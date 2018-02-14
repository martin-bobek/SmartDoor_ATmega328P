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
#define MSK(BIT)        (1u << (BIT))

// PORT B PINS


// PORT C PINS
#define TWI_SDA_C       4
#define TWI_SCL_C       5

// PORT D PINS
#define RTC_IO_D        2
#define RTC_SCLK_D      3
#define RTC_CE_D        4
#define SERVO_2_PIND    5
#define SERVO_1_PIND    6

#define TCCR2A_SETUP    MSK(WGM21)
#define TCCR2B_SETUP    (MSK(CS21) | MSK(CS20))

#endif
