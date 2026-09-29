#ifndef TIMER_H
#define TIMER_H
#include <avr/io.h>

void Timer0_init(void);
void delay_1ms(uint16_t delay);

void Timer1_init(void);
void Timer1_PWM_245(uint8_t Duty_Cycle);

void ramp_up_delay_n_steps(uint8_t start,
                           uint8_t end,
                           uint16_t ms_time,
                           uint8_t num_steps);
#endif