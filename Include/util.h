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
#define CAT(LHS, RHS)   LHS ## RHS

#define TINY_BUTTON1		MSK(0)
#define TINY_BUTTON2		MSK(1)
#define TINY_BUTTON3		MSK(2)
#define TINY_BUTTON4		MSK(3)
#define TINY_IR1			MSK(4)
#define TINY_IR2			MSK(5)
#define TINY_IR3			MSK(6)
#define TINY_IR4			MSK(7)
extern volatile uint8_t G_TinyStatus;

// PORT B PINS
#define RFID_RESET_B	0
#define SERVO_3_B		1
#define SERVO_4_B		2
#define SPI_MOSI_B      3
#define SPI_MISO_B      4
#define SPI_SCK_B       5
#define XTAL1_B			6
#define XTAL2_B			7

// PORT C PINS
#define PET_RFID_C      0
#define HALL_PET_C		1
#define HALL_DOOR_C		2
#define HALL_MAIL_C		3
#define TWI_SDA_C       4
#define TWI_SCL_C       5
#define RESET_C			6

// PORT D PINS
#define ALARM_D			0
#define DOOR_RFID_D		1
#define RTC_CE_D        2
#define RTC_SCLK_D      3
#define UNLOCKED_D		4
#define SERVO_2_D    	5
#define SERVO_1_D    	6
#define RTC_IO_D        7

#define TCCR2A_SETUP    MSK(WGM21)
#define TCCR2B_SETUP    (MSK(CS21) | MSK(CS20))

#define ALARM_ON()				(REG(DDRD).Bit0 = 0)
#define ALARM_OFF()				(REG(DDRD).Bit0 = 1)
#define ALARM_ISON()			(REG(DDRD).Bit0 == 0)
#define UNLOCKED_ON()          	(REG(PORTD).Bit4 = 1)
#define UNLOCKED_OFF()         	(REG(PORTD).Bit4 = 0)

#endif
