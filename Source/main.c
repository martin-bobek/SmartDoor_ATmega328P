#include "util.h"
#include "twi.h"
#include "lcd.h"
#include "servo.h"
#include "rtc.h"

#define HEARTBEAT_ON()          (REG(PORTB).Bit0 = 1)
#define HEARTBEAT_OFF()         (REG(PORTB).Bit0 = 0)

static volatile uint8_t G_u8SysTick = 0;
static uint8_t G_u8ExpectedSysTick = 0;

static inline void SystemInit(void);
static inline void SystemSleep(void);

static void TimeThread(void);
static void ServoThread(void);

__attribute__ ((OS_main)) int main(void) {
  SystemInit();
  
  while (1) {
    HEARTBEAT_ON(); 
    
    RtcService();
    ServoService();
    TwiService();
    LcdService();
    
    ServoThread();
    TimeThread();
    
    /*
    if (flashes == 0) {
      switch (G_TwiError) {
      case TWI_STARTFAIL:
        flashes = 2;
        break;
      case TWI_ADDRESSFAIL:
        flashes = 4;
        break;
      case TWI_DATAFAIL:
        flashes = 6;
        break;
      }
    }
    
    if (flashes != 0) {
      led_tick++;
      if (led_tick == 50) {
        led_tick = 0;
        flashes--;
        PINB_Bit1 = 1;
      }
    }
    */ 
    
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

static void ServoThread(void) {
  static uint8_t servoPos = 63;
  static uint8_t servoTick = 0;
  static uint8_t servoDir = 0;
  
  if (servoTick == 20) {
  servoTick = 0;
  if (servoDir == 0) {
    servoPos++;
    if (servoPos == MAX_POS)
      servoDir = 1;
  }
  else {
    servoPos--;
    if (servoPos == 0)
      servoDir = 0;
  }
    ServoPosition(SERVO_1, servoPos);
    ServoPosition(SERVO_2, servoPos);
  }
  servoTick++;
}

inline static void SystemInit(void) {
  CLKPR = MSK(CLKPCE);                  // CLOCK SETUP
  CLKPR = 0;                            // increases processor speed to 8MHz
  
  DDRB = MSK(1) | MSK(0);               // GPIO SETUP
  SMCR = MSK(SE);                       // ENABLE SLEEP
  
  PORTC = MSK(TWI_SDA_C) | MSK(TWI_SCL_C);              // TWI SETUP
  TWBR = 32;
  TWCR = MSK(TWINT) | TWI_ON;
  
  DDRD = MSK(SERVO_1_PIND) | MSK(SERVO_2_PIND) | MSK(RTC_CE_D) | MSK(RTC_SCLK_D) | MSK(RTC_IO_D);     // SERVO AND RTC SETUP
  
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
    cli(); // Dissables interrupts
    while(1);
  }
  
  while (G_u8SysTick == G_u8ExpectedSysTick)
    asm("SLEEP");  // enter sleep and wait for interrupt
  
  G_u8ExpectedSysTick++;
}

ISR(TIMER2_COMPA_vect) {
  G_u8SysTick++;
}
