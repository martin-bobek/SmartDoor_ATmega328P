#include "mfrc.h"
#include "mfrc_reg.h"
#include "spi.h"

uint8_t G_MfrcTestFlag;

static void Init(void);
static void Request(void);
static void Timeout(void);
static void FillFifo(void);
static void Receive(void);
void (*MfrcService)(void) = Timeout;
static void (*ReturnPtr)(void) = Init;

static uint16_t timeoutCounter = 1000;
static uint8_t spiSize;

static uint8_t buffer[11];
static uint8_t * const cmdBuffer = buffer;              // MUST BE RESTRICTED TO ONLY HOLD 2 BYTES
static uint8_t * const dataBuffer = buffer + 2;         // MUST BE RESTRICTED TO ONLY HOLD 9 BYTES

static void Init(void) {
  static uint8_t state = 0;
  
  switch (state) {
  case 0:
    cmdBuffer[0] = ADDRESS(WRITE, TMODE_REG);
    cmdBuffer[1] = 0x80;           // Timer automatically starts at the end of transmission in Transceive mode
    spiSize = 2; state++;
  case 1: break;                // intentional fallthrough
  case 2:
    cmdBuffer[0] = ADDRESS(WRITE, TPRESCALER_REG);
    cmdBuffer[1] = 0xA9;
    spiSize = 2; state++;
  case 3: break;
  case 4:
    cmdBuffer[0] = ADDRESS(WRITE, TRELOAD_REGH);
    cmdBuffer[1] = 0x03;
    spiSize = 2; state++;
  case 5: break;
  case 6:
    cmdBuffer[0] = ADDRESS(WRITE, TRELOAD_REGL);
    cmdBuffer[1] = 0xE8;        // sets the timer timeout to 25ms;
    spiSize = 2; state++;
  case 7: break;
  case 8:
    cmdBuffer[0] = ADDRESS(WRITE, TXASK_REG);
    cmdBuffer[1] = 0x40;        // sets ask modulation to 100%
    spiSize = 2; state++;
  case 9: break;
  case 10:
    cmdBuffer[0] = ADDRESS(WRITE, MODE_REG);
    cmdBuffer[1] = 0x3D;        // sets up the crc
    spiSize = 2; state++;
  case 11: break;
  case 12:
    cmdBuffer[0] = ADDRESS(WRITE, TXCONTROL_REG);
    cmdBuffer[1] = 0x83;          // enables rf on antenna pins
    spiSize = 2; state++;
  case 13: break;
  case 14:
    cmdBuffer[0] = ADDRESS(WRITE, COLL_REG);
    cmdBuffer[1] = 0;
    spiSize = 2; state++;
  case 15: break;
  case 16:
    MfrcService = Request;
    G_MfrcTestFlag = 1;
    return;
  }
  
  if (spiTransfer(SPI_RFID, &spiSize, cmdBuffer))  // attempt to send current message. if success, move to next state
    state++;
}
static void Request(void) {
  static uint8_t state = 0;
  
  switch (state) {
  case 0:
    cmdBuffer[0] = ADDRESS(WRITE, COMMAND_REG);
    cmdBuffer[1] = TRANSCEIVE_CMD;
    spiSize = 2; state++;
  case 1: break;
  case 2:
    dataBuffer[0] = 0x26;               // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
    spiSize = 1; state++;
    FillFifo();
    return;
  case 3:
    cmdBuffer[0] = ADDRESS(WRITE, COMIRQ_REG);
    cmdBuffer[1] = 0x7F;                // clear any pending interrupts
    spiSize = 2; state++;
  case 4: break;
  case 5:
    cmdBuffer[0] = ADDRESS(WRITE, BITFRAMING_REG);
    cmdBuffer[1] = 0x87;                // starts transmission, with short frame (7 bit)
    spiSize = 2; state++;
  case 6: break;
  case 7:
    state = 0;
    Receive();
    return;
  }
    
  if (spiTransfer(SPI_RFID, &spiSize, cmdBuffer))
    state++;
}
static void Timeout(void) {
  if (timeoutCounter == 0) {
    MfrcService = ReturnPtr;
    return;
  }
  timeoutCounter--;
}       
static void Receive(void) {
  static uint8_t state = 0;
  
  switch (state) {
  case 2:
    if (cmdBuffer[1] & 0x11) { // timeout or idle interrupt
      G_MfrcTestFlag = 2;
      state = 0;
      MfrcService = Timeout;
      return;
    }
    if (cmdBuffer[1] & 0x20) { // receive interrupt
      cmdBuffer[0] = ADDRESS(READ, ERROR_REG);
      cmdBuffer[1] = 0;
      spiSize = 2; state++;
      break;
    }
    state = 0;
  case 0:               // intentional fallthrough
    ReturnPtr = Request;
    MfrcService = Receive;
    timeoutCounter = 500;
    cmdBuffer[0] = ADDRESS(READ, COMIRQ_REG);
    cmdBuffer[1] = 0;
    spiSize = 2; state++;
    break;
  case 4:
    if (cmdBuffer[1] & 0x13 ||  // General errors
        cmdBuffer[1] & 0x08) {  // Collision error
      G_MfrcTestFlag = 2;
      state = 0;
      MfrcService = Timeout;
      return;
    }
    cmdBuffer[0] = ADDRESS(READ, CONTROL_REG);
    cmdBuffer[1] = 0;
    spiSize = 2; state++;
    break;
  case 6:
    if (cmdBuffer[1] & 0x07) {  // only a partial byte received
      G_MfrcTestFlag = 2;
      state = 0;
      MfrcService = Timeout;
      return;
    }
    cmdBuffer[0] = ADDRESS(READ, FIFOLEVEL_REG);
    cmdBuffer[1] = 0;
    spiSize = 2; state++;
    break;
  case 8:
    state = 0;
    MfrcService = Timeout;
    if (cmdBuffer[1] != 2)  // must receive only 2 bytes on a request
      G_MfrcTestFlag = 2;
    else
      G_MfrcTestFlag = 3;
    return;
  }
  
  if (spiTransfer(SPI_RFID, &spiSize, cmdBuffer))
    state++;
}
static void FillFifo(void) {
  static uint8_t state = 0;
  static uint8_t fifo_spiSize = 0;
  
  switch (state) {
  case 0:
    ReturnPtr = MfrcService;
    MfrcService = FillFifo;
    cmdBuffer[0] = ADDRESS(WRITE, FIFOLEVEL_REG);
    cmdBuffer[1] = 0x80;                                // clears and resets the fifo
    fifo_spiSize = 2; 
    state++;
  case 1:
    if (spiTransfer(SPI_RFID, &fifo_spiSize, cmdBuffer))
      state++;
    break;
  case 2:
    cmdBuffer[1] = ADDRESS(WRITE, FIFODATA_REG);
    spiSize++; state++;
  case 3:
    if (spiTransfer(SPI_RFID, &spiSize, &cmdBuffer[1]))
      state++;
    break;
  case 4:
    if (spiSize != 0)
      break;
    state = 0;
    MfrcService = ReturnPtr;
    ReturnPtr();
  }
}