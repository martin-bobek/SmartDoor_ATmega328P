#include "rtc.h"

#define MSB     MSK(7)

uint8_t G_Seconds;
uint8_t G_Minutes;
uint8_t G_Hours;

typedef enum { IDLE, ADDRESS, READ, STOP } state_t;

void RtcService(void) {
  static state_t state = IDLE;
  static uint8_t counter = 0;
  static uint8_t seconds;
  static uint8_t minutes;
  static uint8_t data;
  
  switch (state) {
  case IDLE:
    counter++;
    if (counter == 17) {
      counter = 0;
      REG(PORTD).Bit4 = 1;   // pull CE high indicating start of transmission
      REG(PORTD).Bit2 = 1;   // pull IO high indicating a read operation
      TCCR2A = MSK(COM2B1) | MSK(COM2B0) | TCCR2A_SETUP;        // set SCLK pin to set on compare match
      data = 0xBF;
      state = ADDRESS;
    }
    break;
  case ADDRESS:
    counter++;
    if (counter == 8) {
      REG(DDRD).Bit2 = 0;    // Set IO to high impedence
      REG(PORTD).Bit2 = 0;   // Turns off pullup on IO pin
      counter = 0;
      state = READ;
      TCCR2A = MSK(COM2B1) | TCCR2A_SETUP;          // set SCLK pin to clear on compare match
      TCCR2B = MSK(FOC2B) | TCCR2B_SETUP;           // force SCLK pin low
      break;
    }
    data >>= 1;
    if (data & 1)
      REG(PORTD).Bit2 = 1; // pull IO high
    else
      REG(PORTD).Bit2 = 0; // pull IO high
    TCCR2A = MSK(COM2B1) | TCCR2A_SETUP;          // set SCLK pin to clear on compare match
    TCCR2B = MSK(FOC2B) | TCCR2B_SETUP;           // force SCLK pin low
    TCCR2A = MSK(COM2B1) | MSK(COM2B0) | TCCR2A_SETUP;  // set SCLK pin to set on compare match
    break;
  case READ:
    counter++;
    switch (counter) {
    case 8:
      seconds = data;
      break;
    case 16:
      minutes = data;
      break;
    case 24:
      if (REG(PIND).Bit2)    // if IO line is high
        data |= MSB;    // write a bit 7 to high
      REG(PORTD).Bit4 = 0;   // pull CE low, terminating transfer
      counter = 0;
      state = STOP;   
      return;
    }
    if (REG(PIND).Bit2)    // if IO line is high
      data |= MSB;    // write a bit 7 to high
    data >>= 1;       // shift incoming data one bit to the right
    TCCR2A = MSK(COM2B1) | MSK(COM2B0) | TCCR2A_SETUP;          // set SCLK pin to set on compare match
    TCCR2B = MSK(FOC2B) | TCCR2B_SETUP;                         // force SCLK pin low
    TCCR2A = MSK(COM2B1) | TCCR2A_SETUP;                        // set SCLK pin to clear on compare match
    break;
  case STOP:
    REG(DDRD).Bit2 = 1;      // set IO to output
    G_Seconds = seconds & 0x7F;
    G_Minutes = minutes;
    G_Hours = data & 0x3F;
    state = IDLE;
    break;
  }
}
