#include "servo.h"

#define MINIMUM         (uint8_t)125
#define NEUTRAL         (uint8_t)188
#define PRESCALE        (MSK(CS01) | MSK(CS00))

typedef enum { BEGIN = 0, S1_START = 0, S1_STOP = 2, S2_START = 5, S2_STOP = 7, LOOP = 20 } state_t;

static uint8_t position_1 = NEUTRAL;
static uint8_t position_2 = NEUTRAL;

void ServoPosition(servo_t servo, uint8_t pos) {
  if (pos > MAX_POS)
    pos = MAX_POS;
  switch (servo) {
  case SERVO_1:
    position_1 = pos + MINIMUM;
    break;
  case SERVO_2:
    position_2 = pos + MINIMUM;
    break;
  }
}

void ServoService(void) {
  static state_t state = BEGIN;
  
  switch (state) {
  case S1_START:
    TCCR0A = MSK(COM0A1) | MSK(COM0A0);         // set servo 1 pin to set on compare match
    OCR0A = position_1;                         // set time to compare match
    TCCR0B = MSK(FOC0A) | PRESCALE;             // turns on timer and forces an output compare to force pin high
    TCCR0A = MSK(COM0A1);                       // set servo 1 pin to clear on compare match
    break;
  case S1_STOP:
    TCCR0B = MSK(FOC0A);                        // turns off timer and forces an output compare match in case timer hasnt reached position yet.
    TCNT0 = 0;                                  // clears the timer
    break;
  case S2_START:
    TCCR0A = MSK(COM0B1) | MSK(COM0B0);         // set servo 2 pin to set on compare match
    OCR0B = position_2;                         // set time to compare match
    TCCR0B = MSK(FOC0B) | PRESCALE;             // turns on timer and forces an output compare to force pin high
    TCCR0A = MSK(COM0B1);                       // set servo 2 pin to clear on compare match
    break;
  case S2_STOP:
    TCCR0B = MSK(FOC0B);                        // turns off timer and forces an output compare match in case timer hasnt reached position yet.
    TCNT0 = 0;                                  // clears the timer
    break;
  case LOOP: break;
  }
  
  state++;
  if (state == LOOP)
    state = BEGIN;
}
