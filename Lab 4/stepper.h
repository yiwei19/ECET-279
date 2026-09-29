/*
 * Project: ECET 279 Labs 4-5 Washing Machine
 * File: stepper.h
 * Author: Ivy Yi Wei
 * Date: 09/28/2026
 *
 * Description:
 * Public interface for the washing-machine stepper motor module.
 */

#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>

#define AGITATE  'A'
#define SPIN     'S'

void stepper_init(void);
void motor_control(char mode, uint8_t seconds);
void motor_off(void);

#endif
