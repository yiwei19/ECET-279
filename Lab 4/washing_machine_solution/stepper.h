#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>

#define MOTOR_AGITATE  'A'
#define MOTOR_SPIN     'S'
#define MOTOR_OFF      'O'

void stepper_init(void);
void motor_run(char mode, uint8_t seconds);
void motor_off(void);

#endif
