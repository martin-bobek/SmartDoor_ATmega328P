#include "spi.h"

#define SS_LOW(PIN)     (CAT(REG(PORTB).Bit, PIN) = 0)
#define SS_HIGH(PIN)    (CAT(REG(PORTB).Bit, PIN) = 1)

typedef enum { READY, TRANSFERING } state_t;

static state_t state = READY;
static uint8_t *trans_data;
static uint8_t *bytes_remaining;
static uint8_t ss_pin;

uint8_t spiTransfer(spi_device_t device, uint8_t *size_done, uint8_t data[]) {
  if (state != READY)
    return 0;
  if (*size_done == 0)
    return 1;
  trans_data = data;
  bytes_remaining = size_done;
  ss_pin = device;
  switch (ss_pin) {
  case SPI_RFID:
    SS_LOW(SPI_RFID);
    break;
  default:
    return 0;
  }
  state = TRANSFERING;
  SPDR = *trans_data;
  return 1;
}

ISR(SPI_STC_vect) {
  *trans_data = SPDR;
  --*bytes_remaining;
  if (*bytes_remaining == 0) {
    state = READY;
    switch (ss_pin) {
    case SPI_RFID:
      SS_HIGH(SPI_RFID);
      break;
    }
    return;
  }
  trans_data++;
  SPDR = *trans_data;
}
