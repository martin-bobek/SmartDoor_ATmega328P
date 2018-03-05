#include "spi.h"

#define SS_LOW(PIN)     (CAT(REG(PORTB).Bit, PIN) = 0)
#define SS_HIGH(PIN)    (CAT(REG(PORTB).Bit, PIN) = 1)

typedef enum { READY, START, TRANSCEIVE } state_t;

static state_t state = READY;
static uint8_t *send_data;
static uint8_t *receive_data;
static uint8_t *bytes_remaining;
static uint8_t ss_pin;

uint8_t spiTransfer(spi_device_t device, uint8_t *size_done, uint8_t send[], uint8_t receive[]) {
  if (state != READY)
    return 0;
  if (*size_done == 0)
    return 1;
  send_data = send;
  receive_data = receive;
  bytes_remaining = size_done;
  ss_pin = device;
  switch (ss_pin) {
  case SPI_RFID_B:
    SS_LOW(SPI_RFID_B);
    break;
  default:
    return 0;
  }
  state = START;
  SPDR = *send_data;
  return 1;
}

ISR(SPI_STC_vect) {
  if (state == START)
    state = TRANSCEIVE;
  else if (receive_data) {
    *receive_data = SPDR;
    receive_data++;
  }
  --*bytes_remaining;
  if (*bytes_remaining == 0) {
    state = READY;
    switch (ss_pin) {
    case SPI_RFID_B:
      SS_HIGH(SPI_RFID_B);
      break;
    }
    return;
  }
  send_data++;
  SPDR = *send_data;
}
