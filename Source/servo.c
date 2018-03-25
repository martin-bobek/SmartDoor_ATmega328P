#include "servo.h"

#define MINIMUM         (uint8_t)125
#define NEUTRAL         (uint8_t)188
#define PRESCALE0       (MSK(CS01) | MSK(CS00))

typedef enum { BEGIN = 0, S1_START = 0, S1_STOP = 2, S2_START = 5, S2_STOP = 7, S3_START = 10, S3_STOP = 12, S4_START = 15, S4_STOP = 17, LOOP = 20 } state_t;

static uint8_t position_1 = MINIMUM;
static uint8_t position_2 = MINIMUM;
static uint8_t position_3 = MINIMUM;
static uint8_t position_4 = MINIMUM;

void ServoPosition(servo_t servo, uint8_t pos) {
  if (pos > MAX_POS)
    pos = MAX_POS;
  switch (servo) {
  case SERVO_PETIN:
    position_1 = pos + MINIMUM;
    break;
  case SERVO_PETOUT:
    position_2 = pos + MINIMUM;
    break;
  case SERVO_MAIL:
	position_3 = pos + MINIMUM;
	break;
  case SERVO_DOOR:
	position_4 = pos + MINIMUM;
	break;
  }
}

void ServoService(void) {
  static state_t state = BEGIN;
  
  switch (state) {
  case S1_START:
    TCCR0A = MSK(COM0A1) | MSK(COM0A0);         // set servo 1 pin to set on compare match
    OCR0A = position_1;                         // set time to compare match
    TCCR0B = MSK(FOC0A) | PRESCALE0;            // turns on timer and forces an output compare to force pin high
    TCCR0A = MSK(COM0A1);                       // set servo 1 pin to clear on compare match
    break;
  case S1_STOP:
    TCCR0B = MSK(FOC0A);                        // turns off timer and forces an output compare match in case timer hasnt reached position yet.
    TCNT0 = 0;                                  // clears the timer
    break;
  case S2_START:
    TCCR0A = MSK(COM0B1) | MSK(COM0B0);         // set servo 2 pin to set on compare match
    OCR0B = position_2;                         // set time to compare match
    TCCR0B = MSK(FOC0B) | PRESCALE0;            // turns on timer and forces an output compare to force pin high
    TCCR0A = MSK(COM0B1);                       // set servo 2 pin to clear on compare match
    break;
  case S2_STOP:
    TCCR0B = MSK(FOC0B);                        // turns off timer and forces an output compare match in case timer hasnt reached position yet.
    TCNT0 = 0;                                  // clears the timer
    break;
  case S3_START:
	TCCR1A = MSK(COM1A1) | MSK(COM1A0);
	OCR1AL = position_3;
	TCCR1C = MSK(FOC1A);
	TCCR1B = MSK(CS11) | MSK(CS10);
	TCCR1A = MSK(COM1A1);
	break;
  case S3_STOP:
	TCCR1C = MSK(FOC1A);
	TCCR1B = 0;
	TCNT1H = 0;
	TCNT1L = 0;
	break;
  case S4_START:
    TCCR1A = MSK(COM1B1) | MSK(COM1B0);
    OCR1BL = position_4;
    TCCR1C = MSK(FOC1B);
    TCCR1B = MSK(CS11) | MSK(CS10);
    TCCR1A = MSK(COM1B1);
	break;
  case S4_STOP:
	TCCR1C = MSK(FOC1B);
	TCCR1B = 0;
	TCNT1H = 0;
	TCNT1L = 0;
	break;
  case LOOP: break;
  }
  
  state++;
  if (state == LOOP)
    state = BEGIN;
}
