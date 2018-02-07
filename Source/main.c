#include "util.h"
#include "twi.h"
#include "lcd.h"

#define HEARTBEAT_ON()          (REG(PORTB).Bit0 = 1)
#define HEARTBEAT_OFF()         (REG(PORTB).Bit0 = 0)

static volatile uint8_t G_u8SysTick = 0;
static uint8_t G_u8ExpectedSysTick = 0;

static inline void SystemInit(void);
static inline void SystemSleep(void);

__attribute__ ((OS_main)) int main(void) {
  SystemInit();
  
  //uint8_t flashes = 0;
  //uint8_t led_tick = 0;
  uint16_t lcd_tick = 0;
  uint8_t lcd_msg = 0;
  uint8_t lcd_success = 1;
  //PINB_Bit1 = 1;
  while (1) {
    HEARTBEAT_ON(); 
    
    TwiService();
    LcdService();
    
    if (!lcd_success || lcd_tick == 0) {
      if (lcd_tick == 0)
        lcd_tick = 1000;
      switch (lcd_msg) {
      case 0:
        lcd_success = LcdWrite(0, "Hello!");
        break;
      case 1:
        lcd_success = LcdWrite(LINE2_START, "World!");
        break;
      case 2:
        lcd_success = LcdWrite(0, "ENEL  ");
        break;
      case 3:
        lcd_success = LcdWrite(LINE2_START, "400   ");
        break;
      }
      if (lcd_success) {
        lcd_msg++;
        if (lcd_msg == 4)
          lcd_msg = 0;
      }
    }
    lcd_tick--;
    
    
    /**
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

inline static void SystemInit(void) {
  CLKPR = MSK(CLKPCE);
  CLKPR = 0;                            // increases processor speed to 8MHz
  
  DDRB = (MSK(1) | MSK(0));
  SMCR = MSK(SE);
  
  PORTC = MSK(5) | MSK(4);
  TWBR = 32;
  TWCR = MSK(TWINT) | TWI_ON;
  
  OCR0A = 0x7c;
  TCCR0A = MSK(WGM01);
  TIMSK0 = MSK(OCIE0A);
  TCCR0B = MSK(CS01) | MSK(CS00);
  
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

ISR(TIMER0_COMPA_vect) {
  G_u8SysTick++;
}
