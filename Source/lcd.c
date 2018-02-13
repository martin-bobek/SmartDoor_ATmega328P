#include "lcd.h"

static void Init(void);
static void Ready(void);
static void SendAddress(void);
static void SendStr(void);

#define ENABLE_FLAG                     MSK(0)
#define NIBBLE_FLAG                     MSK(1)
#define BYTE_FORM(nibbleEnable, byte)   ((((nibbleEnable) & ENABLE_FLAG) ? 0 : ENABLE) | (((nibbleEnable) & NIBBLE_FLAG) ? ((byte) << 4) : ((byte) & 0xf0)))
#define CMD_BYTE(nibbleEnable, byte)    (uint8_t)(BYTE_FORM(nibbleEnable, byte) | BACKLIGHT)
#define CHAR_BYTE(nibbleEnable, byte)   (uint8_t)(BYTE_FORM(nibbleEnable, byte) | BACKLIGHT | REG_SELECT)

#define ENABLE                          MSK(2)
#define BACKLIGHT                       MSK(3)
#define REG_SELECT                      MSK(0)

#define NORMALIZE                       0x30
#define MODE_NIBBLE                     0x28
#define LCD_ON                          0x0c
#define INC_NOSHIFT                     0x06

#define LINE1_END                       0x28
#define LINE2_END                       0x67

void (*LcdService)(void) = Init;

static uint8_t address;
static char *str;
static uint8_t index = 0;
static uint8_t nibbleEnable = 0;

static void Init(void) {
  uint8_t success = 0;
  const uint8_t startCommands[] = { MODE_NIBBLE, LCD_ON, INC_NOSHIFT };
  while (index < 8) {
    switch (index) {
    case 0:
    case 2:
    case 4:
      success = TwiSend(LCD_ADDR, CMD_BYTE(0, NORMALIZE));
      break;
    case 1:
    case 3:
    case 5:
      success = TwiSend(LCD_ADDR, CMD_BYTE(ENABLE_FLAG, NORMALIZE));
      break;
    case 6:
      success = TwiSend(LCD_ADDR, CMD_BYTE(0, MODE_NIBBLE));
      break;
    case 7:
      success = TwiSend(LCD_ADDR, CMD_BYTE(ENABLE_FLAG, MODE_NIBBLE));
      break;
    }
    if (!success)
      return;
    index++;
  }
  while(index < 10) {
    while (nibbleEnable < 4) {
      if (!TwiSend(LCD_ADDR, CMD_BYTE(nibbleEnable, startCommands[index - 8])))
        return;
      nibbleEnable++;
    }
    nibbleEnable = 0;
    index++;
  }
  index = 0;
  LcdService = Ready;
}
static void Ready(void) {
  
}
static void SendAddress(void) {
  while (nibbleEnable < 4) {
    if (!TwiSend(LCD_ADDR, CMD_BYTE(nibbleEnable, MSK(7) | address)))
      return;
    nibbleEnable++;
  }
  nibbleEnable = 0;
  LcdService = SendStr;
  SendStr();
}
static void SendStr(void) {
  while ((str[index] != '\0') && (address + index != LINE1_END) && (address + index != LINE2_END)) {
    while (nibbleEnable < 4) {
      if (!TwiSend(LCD_ADDR, CHAR_BYTE(nibbleEnable, str[index])))
        return;
      nibbleEnable++;
    }
    nibbleEnable = 0;
    index++;
  }
  index = 0;
  LcdService = Ready;
}

uint8_t LcdWrite(uint8_t startAddr, char message[]) {
  if (LcdService != Ready)
    return 0;
  if ((startAddr >= LINE1_END && startAddr < LINE2_START) || startAddr >= LINE2_END)
    return 0;
  str = message;
  address = startAddr;
  LcdService = SendAddress;
  SendAddress();
  return 1;
}
