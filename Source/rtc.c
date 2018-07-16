#include "rtc.h"

#define MSB             MSK(7)
#define READ_BURST      0xBF
#define WRITE_BURST     0xBE
#define DELAY           16

uint8_t G_Seconds;
uint8_t G_Minutes;
uint8_t G_Hours;
uint8_t G_RtcWrite;

typedef enum { IDLE, WRITE, READ, STOP } state_t;

void RtcService(void) {
  static state_t state = IDLE;
  static uint8_t counter = 0;
  static uint8_t seconds;
  static uint8_t minutes;
  static uint8_t data;
  
  switch (state) {
  case IDLE:
    counter++;
    if (counter == DELAY) {
      counter = 0;
      REG(PORTD).Bit2 = 1;                                           // pull CE high indicating start of transmission
      state = WRITE;
      if (G_RtcWrite) {
    	  G_RtcWrite = SERVICING_FLAG;
    	  data = WRITE_BURST;
      }
      else
    	  data = READ_BURST;
    }
    break;
  case WRITE:
    counter++;
    switch (counter) {
    case 9:
      if (G_RtcWrite & SERVICING_FLAG) {
        data = G_Seconds;
        break;
      }
      REG(DDRD).Bit7 = 0;                                            // Set IO to high impedence
      REG(PORTD).Bit7 = 0;                                           // Turns off pullup on IO pin
      state = READ;
      counter = 0;
      TCCR2A = MSK(COM2B1) | TCCR2A_SETUP;                      // set SCLK pin to clear on compare match
      TCCR2B = MSK(FOC2B) | TCCR2B_SETUP;                       // force SCLK pin low
      return;
    case 17:
      data = G_Minutes;
      break;
    case 25:
      data = G_Hours;
      break;
    case 73:
      REG(PORTD).Bit7 = 0;                                           // Set IO low
      REG(PORTD).Bit2 = 0;                                           // pull CE pin low, ending data transfer
      G_RtcWrite = 0;
      state = IDLE;
      counter = 0;
      return;
    }
    REG(PORTD).Bit7 = (data & 1) ? 1 : 0;                            // write data to IO
    data >>= 1;
    TCCR2A = MSK(COM2B1) | TCCR2A_SETUP;                        // set SCLK pin to clear on compare match
    TCCR2B = MSK(FOC2B) | TCCR2B_SETUP;                         // force SCLK pin low
    TCCR2A = MSK(COM2B1) | MSK(COM2B0) | TCCR2A_SETUP;          // set SCLK pin to set on compare match
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
      if (REG(PIND).Bit7)                                            // if IO line is high
        data |= MSB;                                            // write a bit 7 to high
      REG(PORTD).Bit2 = 0;                                           // pull CE low, terminating transfer
      counter = 0;
      state = STOP;   
      return;
    }
    if (REG(PIND).Bit7)                                              // if IO line is high
      data |= MSB;                                              // write a bit 7 to high
    data >>= 1;                                                 // shift incoming data one bit to the right
    TCCR2A = MSK(COM2B1) | MSK(COM2B0) | TCCR2A_SETUP;          // set SCLK pin to set on compare match
    TCCR2B = MSK(FOC2B) | TCCR2B_SETUP;                         // force SCLK pin low
    TCCR2A = MSK(COM2B1) | TCCR2A_SETUP;                        // set SCLK pin to clear on compare match
    break;
  case STOP:
    REG(DDRD).Bit7 = 1;                                              // set IO to output
    if (!G_RtcWrite) {                                         
      G_Seconds = seconds & 0x7F;                               // Write new time to globals
      G_Minutes = minutes;
      G_Hours = data & 0x3F;
    }
    state = IDLE;
    break;
  }
}
