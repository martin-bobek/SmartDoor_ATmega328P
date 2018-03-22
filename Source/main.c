#include "util.h"
#include "twi.h"
#include "spi.h"
#include "lcd.h"
#include "servo.h"
#include "rtc.h"
#include "lock.h"
#include "pet_door.h"
#include "mfrc.h"
#include "id_check.h"

static volatile uint8_t G_u8SysTick = 0;
static uint8_t G_u8ExpectedSysTick = 0;

static inline void SystemInit(void);
static inline void SystemSleep(void);

static void TimeThread(void);
static void RfidThread(void);
static char HexToAscii(uint8_t hex);

__attribute__ ((OS_main)) int main(void) {
  SystemInit();
  
  while (1) {
    HEARTBEAT_ON(); 
    
    RtcService();
    ServoService();
    TwiService();
    LcdService();
    MfrcService();
    
    TimeThread();
    RfidThread();
    IdCheckThread();
    PetDoorThread();
    LockThread();
    
    HEARTBEAT_OFF();
    SystemSleep();
  }
}

static void TimeThread(void) {
  static char time[] = "  :  :  ";
  static uint8_t prevSec = 0xFF;
  static uint8_t lcdSuccess = 1;
  static uint16_t ticksToChange = 0;
  
  if (ticksToChange <= 5000)
    ticksToChange++;
  if (ticksToChange == 5000) {
    G_Seconds = (3 << 4) | 1;
    G_Minutes = (5 << 4) | 7;
    G_Hours = (1 << 4) | 2;
    G_TimeWrite = CHANGE_FLAG;
  }
  
  if (prevSec != G_Seconds) {
    prevSec = G_Seconds;
    time[0] = (G_Hours >> 4) + '0';
    time[1] = (G_Hours & 0xF) + '0';
    time[3] = (G_Minutes >> 4) + '0';
    time[4] = (G_Minutes & 0xF) + '0';
    time[6] = (G_Seconds >> 4) + '0';
    time[7] = (G_Seconds & 0xF) + '0';
    lcdSuccess = 0;
  }    
  if (!lcdSuccess) {
    lcdSuccess = LcdWrite(0, time);
  }
}

static void RfidThread(void) {
  static uint8_t success = 1;
  static char idStr[] = "              ";
  
  if (success && G_MfrcTestFlag)
    success = 0;
  if (!success) {
    switch (G_MfrcTestFlag) {
    case 1:
      success = LcdWrite(LINE2_START, "Init Done!");
      break;
    case 2:
      success = LcdWrite(LINE2_START, "No Response!  ");
      break;
    case 3:
      success = LcdWrite(LINE2_START, "PICC Detected!");
      break;
    case 4: 
      success = LcdWrite(LINE2_START, "Bad UID!      ");
      break;
    case 5:
      for (uint8_t i = 8; i-- > 0; ) {
        idStr[i] = HexToAscii(G_PiccUid);
        G_PiccUid >>= 4;
      }
      success = LcdWrite(LINE2_START, idStr);
      break;
    }
    if (success)
      G_MfrcTestFlag = 0;
  }
}

static char HexToAscii(uint8_t hex) {
  hex &= 0xF;
  if (hex < 10)
    return hex + '0';
  return hex - 10 + 'A';
}

inline static void SystemInit(void) {
  CLKPR = MSK(CLKPCE);                  // CLOCK SETUP
  CLKPR = MSK(CLKPS0);                  // increases processor speed from 2 to 8MHz
  
  SMCR = MSK(SE);                       // ENABLE SLEEP
  
  PORTC = MSK(SPI_RFID_C) | MSK(TWI_SDA_C) | MSK(TWI_SCL_C);              // TWI SETUP
  DDRC = MSK(SPI_RFID_C);
  TWBR = 32;
  TWCR = MSK(TWINT) | TWI_ON;
  
  DDRD = MSK(SERVO_1_PIND) | MSK(SERVO_2_PIND) | MSK(RTC_CE_D) | MSK(RTC_SCLK_D) | MSK(RTC_IO_D) | MSK(HEARTBEAT_D);     // SERVO AND RTC SETUP

  PORTB = MSK(RFID_RESET_B) | MSK(SPI_MOSI_B) | MSK(SPI_SCK_B);
  DDRB = MSK(RFID_RESET_B) | MSK(SPI_MOSI_B) | MSK(SPI_SCK_B) | MSK(SERVO_3_PINB) | MSK(SERVO_4_PINB);               // GPIO SETUP

  // SPSR = MSK(SPI2X);
  SPCR = MSK(SPIE) | MSK(SPE) | MSK(MSTR) | MSK(SPR1) | MSK(SPR0);      // SPI SETUP
  
  OCR2A = 0xf9;                         // SLEEP TIMER SETUP
  OCR2B = 0x7c;
  TCCR2A = TCCR2A_SETUP;
  TIMSK2 = MSK(OCIE2A);
  TCCR2B = TCCR2B_SETUP;
  
  sei();
}

inline static void SystemSleep(void) {
  if (G_u8SysTick != G_u8ExpectedSysTick)
  {
    cli(); // Disables interrupts
    while(1);
  }
  
  while (G_u8SysTick == G_u8ExpectedSysTick)
    asm("SLEEP");  // enter sleep and wait for interrupt
  
  G_u8ExpectedSysTick++;
}

ISR(TIMER2_COMPA_vect) {
  G_u8SysTick++;
}
