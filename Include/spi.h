#ifndef SPI_H_
#define SPI_H_

#include "util.h"

typedef uint8_t spi_device_t;

#define SPI_RFID        2
#define SPI_MISO        4
#define SPI_MOSI        3
#define SPI_SCK         5

uint8_t spiTransfer(spi_device_t device, uint8_t *size_done, uint8_t data[]);          /// 3 packet messages are guaranteed to be delivered within 1ms (at lowest spi speed), and size_done does not need to be checked

#endif