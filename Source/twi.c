#include "twi.h"

#define READ_BIT				MSK(0)

#define BUF_LEN                 16
#define STAT_MSK                0xf8
#define START_ACK               0x08
#define REPSTART_ACK            0x10
#define ADDRESSWRITE_ACK        0x18
#define DATAWRITE_ACK           0x28
#define ADDRESSREAD_ACK			0x40
#define DATAREAD_ACK			0x58

typedef struct {
  uint8_t address;
  uint8_t data;
} message_t;

volatile twi_error_t G_TwiError = TWI_NOERROR;
volatile uint8_t G_TinyStatus;

typedef enum { READY, START, STALE_START, ADDRESS, DATA } twi_state_t;

static volatile twi_state_t twi_state = READY;
static volatile message_t data_buffer[BUF_LEN];
static volatile uint8_t trans_pos = 0;
static uint8_t write_pos = 0;

uint8_t TwiSend(uint8_t address, uint8_t data)
{
  uint8_t new_pos = write_pos + 1;
  if (new_pos == BUF_LEN)
    new_pos = 0;
  if (new_pos == trans_pos)
    return 0;
  
  data_buffer[write_pos].address = address;
  data_buffer[write_pos].data = data;
  write_pos = new_pos;
  
  if (twi_state == READY) {
    twi_state = START;
    TWCR = MSK(TWINT) | MSK(TWSTA) | TWI_ON;
  }
  return 1;
}

void TwiService(void) {
  uint8_t new_pos = write_pos + 1;
  if (new_pos == BUF_LEN)
	new_pos = 0;
  if (new_pos != trans_pos) {
	data_buffer[write_pos].address = TINY_ADDR;
	write_pos = new_pos;
  }

  if (twi_state == START)
	twi_state = STALE_START;
  else if (twi_state == STALE_START) {
	TWCR = 0;
	TWCR = MSK(TWINT) | MSK(TWSTA) | TWI_ON;
  }
  else if (twi_state == READY) {
    twi_state = START;
    TWCR = MSK(TWINT) | MSK(TWSTA) | TWI_ON;
  }
}

ISR(TWI_vect) {
  switch (twi_state) {
  case START:
  case STALE_START: {
    uint8_t status = TWSR & STAT_MSK;
    if (status == START_ACK || status == REPSTART_ACK) {
      twi_state = ADDRESS;
	  TWDR = data_buffer[trans_pos].address;
	  TWCR = MSK(TWINT) | TWI_ON;
	  return;
    }
    twi_state = READY;
    G_TwiError = TWI_STARTFAIL;
    TWCR = MSK(TWINT) | MSK(TWSTO) | TWI_ON;
    return;
  }
  case ADDRESS:
    if ((data_buffer[trans_pos].address & READ_BIT) && (TWSR & STAT_MSK) == ADDRESSREAD_ACK) {
      twi_state = DATA;
      TWCR = MSK(TWINT) | TWI_ON;
      return;
    }
    if (!(data_buffer[trans_pos].address & READ_BIT) && (TWSR & STAT_MSK) == ADDRESSWRITE_ACK) {
      twi_state = DATA;
      TWDR = data_buffer[trans_pos].data;
      TWCR = MSK(TWINT) | TWI_ON;
	  return;
    }
    G_TwiError = TWI_ADDRESSFAIL;
    twi_state = READY;
    TWCR = MSK(TWINT) | MSK(TWSTO) | TWI_ON;
    return;
  case DATA: {
    uint8_t address = data_buffer[trans_pos].address;
    if (((address & READ_BIT) && (TWSR & STAT_MSK) == DATAREAD_ACK) ||
       (!(address & READ_BIT) && (TWSR & STAT_MSK) == DATAWRITE_ACK)) {
      if (address & READ_BIT)
        G_TinyStatus = TWDR;
      trans_pos++;
      if (trans_pos == BUF_LEN)
        trans_pos = 0;
      if (trans_pos != write_pos) {
        if (!(address & READ_BIT) && address == data_buffer[trans_pos].address) {
          TWDR = data_buffer[trans_pos].data;
          TWCR = MSK(TWINT) | TWI_ON;
          return;
        }
        twi_state = START;
        TWCR = MSK(TWINT) | MSK(TWSTA) | TWI_ON;
        return;
      }
    }
    else {
      G_TwiError = TWI_DATAFAIL;
    }
    twi_state = READY;
    TWCR = MSK(TWINT) | MSK(TWSTO) | TWI_ON;
    return;
  }
  default:
    while(1);
  }
}
