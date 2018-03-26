#include "util.h"
#include "twi.h"
#include "spi.h"
#include "lcd.h"
#include "servo.h"
#include "rtc.h"
#include "lock.h"
#include "pet_door.h"
#include "main_door.h"
#include "mfrc.h"
#include "id_check.h"
#include "hall.h"
#include "buttons.h"
#include "times.h"
#include "display.h"

static volatile uint8_t G_u8SysTick = 0;
static uint8_t G_u8ExpectedSysTick = 0;

static inline void SystemInit(void);
static inline void SystemSleep(void);

//static void TimeThread(void);
//static void RfidThread(spi_device_t device);
//static void ServoThread(void);
static void HallThread(void);
static char HexToAscii(uint8_t hex);

/*
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
}
*/

__attribute__ ((OS_main)) int main(void) {
  SystemInit();
  
  while (1) {
    //HEARTBEAT_ON();
    
    RtcService();
    ServoService();
    TwiService();
    LcdService();
    MfrcService();
    AdcService();
    HallService();
    ButtonService();
    IdLogService();

    TimesThread();
    //ServoThread();
    //RfidThread(PET_SPI);
    //RfidThread(DOOR_SPI);
    IdCheckThread();
    PetDoorThread();
    LockThread();
    //DisplayThread();
    MainDoorThread();
    HallThread();

    //HEARTBEAT_OFF();
    SystemSleep();
  }
}

static void HallThread(void) {
	static char message[] = "  -  -  :   ";
	static uint8_t prevValue[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
	static uint8_t success = 1;
	static uint8_t counter = 0;

	counter++;
	if (counter == 100) {
		counter = 0;

		if (prevValue[0] != G_HallValue[0]) {
			prevValue[0] = G_HallValue[0];
			message[1] = HexToAscii(prevValue[0]);
			message[0] = HexToAscii(prevValue[0] >> 4);
			success = 0;
		}
		if (prevValue[1] != G_HallValue[1]) {
			prevValue[1] = G_HallValue[1];
			message[4] = HexToAscii(prevValue[1]);
			message[3] = HexToAscii(prevValue[1] >> 4);
			success = 0;
		}
		if (prevValue[2] != G_HallValue[2]) {
			prevValue[2] = G_HallValue[2];
			message[7] = HexToAscii(prevValue[2]);
			message[6] = HexToAscii(prevValue[2] >> 4);
			success = 0;
		}
		if (prevValue[3] != G_DoorClosed) {
			prevValue[3] = G_DoorClosed;
			message[9] = (prevValue[3] & MAIN_CLOSED) ? '1' : '0';
			message[10] = (prevValue[3] & PET_CLOSED) ? '1' : '0';
			message[11] = (prevValue[3] & MAIL_CLOSED) ? '1' : '0';
			success = 0;
		}
	}
	if (!success)
		success = LcdWrite(LINE2_START, message);
}

/*
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
    ServoPosition(SERVO_MAIL, servoPos);
    ServoPosition(SERVO_DOOR, servoPos);
  }
  servoTick++;
}
*/

static char HexToAscii(uint8_t hex) {
  hex &= 0xF;
  if (hex < 10)
    return hex + '0';
  return hex - 10 + 'A';
}

/*
static void RfidThread(spi_device_t device) {
  static const uint8_t lcdAddress[NUM_RFID] = { 0, LINE2_START };
  static uint8_t success[NUM_RFID] = { 1, 1 };
  static char idStr[NUM_RFID][15] = { "              ", "              " };
  
  if (success[device] && G_MfrcTestFlag[device])
    success[device] = 0;
  if (!success[device]) {
    switch (G_MfrcTestFlag[device]) {
    case 1:
      success[device] = LcdWrite(lcdAddress[device], "Init Done!");
      break;
    case 2:
      success[device] = LcdWrite(lcdAddress[device], "No Response!  ");
      break;
    case 3:
      success[device] = LcdWrite(lcdAddress[device], "PICC Detected!");
      break;
    case 4: 
      success[device] = LcdWrite(lcdAddress[device], "Bad UID!      ");
      break;
    case 5: {
      uint32_t piccUid = G_PiccUid[device];
      for (uint8_t i = 8; i-- > 0; ) {
        idStr[device][i] = HexToAscii(piccUid);
        piccUid >>= 4;
      }
      success[device] = LcdWrite(lcdAddress[device], idStr[device]);
      break;
    }
    }
    if (success[device])
      G_MfrcTestFlag[device] = 0;
  }
}
*/
inline static void SystemInit(void) {
  CLKPR = MSK(CLKPCE);                  // CLOCK SETUP
  CLKPR = MSK(CLKPS0);                  // increases processor speed from 2 to 8MHz
  
  InitTimes();
  InitId();

  SMCR = MSK(SE);                       // ENABLE SLEEP
  
  DIDR0 = MSK(ADC3D) | MSK(ADC2D) | MSK(ADC1D);		// ADC SETUP
  ADMUX = ADMUX_SETUP;
  ADCSRA = ADCSRA_SETUP;

  PORTC = MSK(PET_RFID_C) | MSK(TWI_SDA_C) | MSK(TWI_SCL_C);              // TWI SETUP
  DDRC = MSK(PET_RFID_C);
  TWBR = 32;
  TWCR = MSK(TWINT) | TWI_ON;
  
  PORTD = MSK(DOOR_RFID_D);
  DDRD = MSK(ALARM_D) | MSK(DOOR_RFID_D) | MSK(SERVO_1_D) | MSK(SERVO_2_D) | MSK(RTC_CE_D) | MSK(RTC_SCLK_D) | MSK(RTC_IO_D) | MSK(HEARTBEAT_D);     // SERVO AND RTC SETUP

  PORTB = MSK(RFID_RESET_B) | MSK(SPI_MOSI_B) | MSK(SPI_SCK_B);
  DDRB = MSK(RFID_RESET_B) | MSK(SPI_MOSI_B) | MSK(SPI_SCK_B) | MSK(SERVO_3_B) | MSK(SERVO_4_B);               // GPIO SETUP

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
  if (G_u8SysTick != G_u8ExpectedSysTick) {
	HEARTBEAT_ON();
    //cli(); // Disables interrupts
    //while(1);
  }
  
  while (G_u8SysTick == G_u8ExpectedSysTick)
    asm("SLEEP");  // enter sleep and wait for interrupt
  
  G_u8ExpectedSysTick++;
}

ISR(TIMER2_COMPA_vect) {
  G_u8SysTick++;
}
