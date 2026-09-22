/*
 * File name: stepper.c
 * Purpose: Washing-machine agitate and spin motor operations.
 * Hardware: PD0-PD3 connect to ULN2003 IN1-IN4.
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "stepper.h"

#define STEPPER_PORT        PORTD
#define STEPPER_DDR         DDRD
#define STEPPER_MASK        0x0F

#define AGITATE_STEP_DELAY_MS  4

/*
 * The operational-requirements table specifies 2 ms for spin. Procedure 3
 * says 4 ms. This solution follows the table. Change this value to 4 if the
 * instructor requires the Procedure 3 value instead.
 */
#define SPIN_STEP_DELAY_MS     2

static const uint8_t half_step_pattern[8] =
{
    0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08
};

static const uint8_t full_step_pattern[4] =
{
    0x03, 0x06, 0x0C, 0x09
};

static void write_step(uint8_t pattern);
static void agitate_one_second(uint8_t clockwise);
static void spin_one_second(void);

void stepper_init(void)
{
    STEPPER_DDR |= STEPPER_MASK;
    motor_off();
}

void motor_run(char mode, uint8_t seconds)
{
    uint8_t elapsed_second;

    if (mode == MOTOR_AGITATE)
    {
        /* Reverse direction after every one-second section. */
        for (elapsed_second = 0; elapsed_second < seconds; elapsed_second++)
        {
            agitate_one_second((elapsed_second % 2) == 0);
        }
    }
    else if (mode == MOTOR_SPIN)
    {
        for (elapsed_second = 0; elapsed_second < seconds; elapsed_second++)
        {
            spin_one_second();
        }
    }

    /* MOTOR_OFF and all invalid modes reach this safe default. */
    motor_off();
}

void motor_off(void)
{
    STEPPER_PORT &= (uint8_t)~STEPPER_MASK;
}

static void write_step(uint8_t pattern)
{
    /* Preserve PD4-PD7 while replacing only the four stepper bits. */
    STEPPER_PORT = (STEPPER_PORT & 0xF0) | (pattern & STEPPER_MASK);
}

static void agitate_one_second(uint8_t clockwise)
{
    uint16_t step;
    uint8_t index;

    /* 1000 ms / 4 ms per half-step = 250 half-steps per second. */
    for (step = 0; step < (1000 / AGITATE_STEP_DELAY_MS); step++)
    {
        index = step % 8;

        if (!clockwise)
        {
            index = 7 - index;
        }

        write_step(half_step_pattern[index]);
        _delay_ms(AGITATE_STEP_DELAY_MS);
    }
}

static void spin_one_second(void)
{
    uint16_t step;
    uint8_t index;

    /* Full-step clockwise operation at the configured inter-step delay. */
    for (step = 0; step < (1000 / SPIN_STEP_DELAY_MS); step++)
    {
        index = step % 4;
        write_step(full_step_pattern[index]);
        _delay_ms(SPIN_STEP_DELAY_MS);
    }
}
