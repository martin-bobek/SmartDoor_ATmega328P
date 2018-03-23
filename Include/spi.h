#ifndef SPI_H_
#define SPI_H_

#include "util.h"

#define NUM_RFID			2

typedef enum { PET_SPI, DOOR_SPI } spi_device_t;

uint8_t spiTransfer(spi_device_t device, uint8_t *size_done, uint8_t send[], uint8_t receive[]);          /// 3 packet messages are guaranteed to be delivered within 1ms (at lowest spi speed), and size_done does not need to be checked

#endif
