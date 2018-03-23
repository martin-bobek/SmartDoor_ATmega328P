#ifndef SERVO_H_
#define SERVO_H_

#include "util.h"

typedef enum { SERVO_PETIN, SERVO_PETOUT, SERVO_3, SERVO_4 } servo_t;

#define SERVO_1_D            6
#define SERVO_2_D            5
#define MAX_POS              125

void ServoPosition(servo_t servo, uint8_t pos);
void ServoService(void);

#endif
