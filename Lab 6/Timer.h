#ifndef TIMER_H
#define TIMER_H
#include <avr/io.h>

void Timer0_init(void);
void delay_1ms(uint16_t delay);

void Timer1_init(void);
void Timer1_PWM_245(uint8_t Duty_Cycle);

#endif