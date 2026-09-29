/*
 * Project: ECET 279 Labs 4-5 Washing Machine
 * File: stepper.c
 * Author: Ivy Yi Wei
 * Date: 09/28/2026
 *
 * Description:
 * Controls the washing-machine stepper motor. Agitate uses half
 * steps and reverses every second. Spin uses clockwise full steps.
 * Both modes use a 4 ms delay between steps.
 *
 * Hardware: PD0-PD3 connect to stepper driver IN1-IN4.
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "stepper.h"

#define MOTOR_MASK  0x0F
#define STEP_DELAY  4
#define STEPS_SEC   (1000 / STEP_DELAY)

static const uint8_t half_step[8] =
{
    0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08
};

static const uint8_t full_step[4] =
{
    0x03, 0x06, 0x0C, 0x09
};

static void write_step(uint8_t pattern);

void stepper_init(void)
{
    DDRD |= MOTOR_MASK;
    motor_off();
}

void motor_control(char mode, uint8_t seconds)
{
    uint8_t second;
    uint16_t step;
    uint8_t index;

    for (second = 0; second < seconds; second++)
    {
        for (step = 0; step < STEPS_SEC; step++)
        {
            if (mode == AGITATE)
            {
                index = step % 8U;

                /* Reverse direction every second. */
                if ((second % 2U) != 0U)
                {
                    index = 7U - index;
                }

                write_step(half_step[index]);
            }
            else if (mode == SPIN)
            {
                index = step % 4U;
                write_step(full_step[index]);
            }
            else
            {
                motor_off();
                return;
            }

            _delay_ms(STEP_DELAY);
        }
    }

    motor_off();
}

void motor_off(void)
{
    PORTD &= (uint8_t)~MOTOR_MASK;
}

static void write_step(uint8_t pattern)
{
    /* Preserve PD4-PD7 and change only the motor bits. */
    PORTD = (PORTD & 0xF0) | (pattern & MOTOR_MASK);
}
