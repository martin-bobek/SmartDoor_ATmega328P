#ifndef LCD_H__
#define LCD_H__

#include "util.h"
#include "twi.h"

#define LINE2_START     0x40

void Lcd_Service(void);

extern void (*LcdService)(void);
uint8_t LcdWrite(uint8_t startAddr, char str[]);

#endif