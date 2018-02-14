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

__attribute__ ((OS_main)) int main(void) {
  SystemInit();
  
  //uint8_t flashes = 0;
  //uint8_t led_tick = 0;
  uint8_t prev_sec = 0xFF;
  char time[] = "  :  :  ";
  uint8_t lcd_msg = 0;
  uint8_t lcd_success = 1;
  uint8_t servo_pos = 63;
  uint8_t servo_tick = 0;
  uint8_t servo_dir = 0;
  //PINB_Bit1 = 1;
  while (1) {
    HEARTBEAT_ON(); 
    
    RtcService();
    ServoService();
    TwiService();
    LcdService();
    
    if (servo_tick == 20) {
      servo_tick = 0;
      if (servo_dir == 0) {
        servo_pos++;
        if (servo_pos == MAX_POS)
          servo_dir = 1;
      }
      else {
        servo_pos--;
        if (servo_pos == 0)
          servo_dir = 0;
      }
      ServoPosition(SERVO_1, servo_pos);
      ServoPosition(SERVO_2, servo_pos);
    }
    servo_tick++;
    
    
    if (!lcd_success || (prev_sec != G_Seconds)) {
      if (prev_sec != G_Seconds) {
        prev_sec = G_Seconds;
        time[0] = (G_Hours >> 4) + '0';
        time[1] = (G_Hours & 0xF) + '0';
        time[3] = (G_Minutes >> 4) + '0';
        time[4] = (G_Minutes & 0xF) + '0';
        time[6] = (G_Seconds >> 4) + '0';
        time[7] = (G_Seconds & 0xF) + '0';
      }
      lcd_success = LcdWrite(0, time);
      /*switch (lcd_msg) {
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
      }*/
      if (lcd_success) {
        lcd_msg++;
        if (lcd_msg == 4)
          lcd_msg = 0;
      }
    }
    //lcd_tick--;
    
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
