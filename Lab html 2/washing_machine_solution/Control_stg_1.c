/*
 * Project name: Wash_MC_Sys_Dev
 * File name: Control_stg_1.c
 * Author: Ivy Yi Wei
 * Date: September 15, 2026
 *
 * Description:
 * This program initializes and tests all inputs and outputs used by the
 * washing-machine controller. It first tests each output. It then continuously
 * reads the five active-low inputs and displays their states on the status and
 * valve outputs.
 *
 * ATmega2560 hardware connections:
 * PA0 - Hot temperature switch (active low)
 * PA1 - Warm temperature switch (active low)
 * PA2 - Cold temperature switch (active low)
 * PA3 - Door-open switch (active low when the door is open)
 * PA4 - Start pushbutton (active low)
 * PD0 - ULN2003 IN1
 * PD1 - ULN2003 IN2
 * PD2 - ULN2003 IN3
 * PD3 - ULN2003 IN4
 * PC0 - Drain-valve LED
 * PC1 - Hot-water-valve LED
 * PC2 - Cold-water-valve LED
 * PC3 - Wash-done LED
 * PC4 - Agitate LED
 * PC5 - Spin LED
 * Timer 0 / util/delay - short test delays
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#if defined(USE_DEBUGGER)
#include "Debugger.h"
#endif

#define HOT_SELECT_BIT       PA0
#define WARM_SELECT_BIT      PA1
#define COLD_SELECT_BIT      PA2
#define DOOR_OPEN_BIT        PA3
#define START_BUTTON_BIT     PA4
#define INPUT_MASK           0x1F

#define DRAIN_VALVE_BIT      PC0
#define HOT_VALVE_BIT        PC1
#define COLD_VALVE_BIT       PC2
#define DONE_LED_BIT         PC3
#define AGITATE_LED_BIT      PC4
#define SPIN_LED_BIT         PC5
#define CONTROL_OUTPUT_MASK  0x3F

#define STEPPER_MASK         0x0F

static void io_init(void);
static uint8_t input_is_active(uint8_t bit_number);
static void test_outputs(void);

int main(void)
{
    io_init();

#if defined(USE_DEBUGGER)
    initDebug();
#endif

    test_outputs();

    while (1)
    {
        /* Clear all indicators before displaying the current input states. */
        PORTC &= (uint8_t)~CONTROL_OUTPUT_MASK;

        if (input_is_active(HOT_SELECT_BIT))
        {
            PORTC |= (1 << HOT_VALVE_BIT);
        }

        if (input_is_active(WARM_SELECT_BIT))
        {
            PORTC |= (1 << HOT_VALVE_BIT) | (1 << COLD_VALVE_BIT);
        }

        if (input_is_active(COLD_SELECT_BIT))
        {
            PORTC |= (1 << COLD_VALVE_BIT);
        }

        if (input_is_active(DOOR_OPEN_BIT))
        {
            PORTC |= (1 << DONE_LED_BIT);
        }

        if (input_is_active(START_BUTTON_BIT))
        {
            PORTC |= (1 << AGITATE_LED_BIT);
        }
    }
}

static void io_init(void)
{
    /* Configure PA0-PA4 as inputs and enable their internal pull-up resistors. */
    DDRA &= (uint8_t)~INPUT_MASK;
    PORTA |= INPUT_MASK;

    /* Configure the six valve/status pins as outputs and turn them off. */
    DDRC |= CONTROL_OUTPUT_MASK;
    PORTC &= (uint8_t)~CONTROL_OUTPUT_MASK;

    /* Configure PD0-PD3 as stepper outputs and turn all motor coils off. */
    DDRD |= STEPPER_MASK;
    PORTD &= (uint8_t)~STEPPER_MASK;
}

static uint8_t input_is_active(uint8_t bit_number)
{
    /* Pull-ups make an active switch or pressed button read as logic zero. */
    return (PINA & (1 << bit_number)) == 0;
}

static void test_outputs(void)
{
    uint8_t bit_number;

    /* Test each valve and status output one at a time. */
    for (bit_number = 0; bit_number < 6; bit_number++)
    {
        PORTC = (PORTC & (uint8_t)~CONTROL_OUTPUT_MASK) | (1 << bit_number);
        _delay_ms(250);
    }
    PORTC &= (uint8_t)~CONTROL_OUTPUT_MASK;

    /* Test each stepper-driver input one at a time. */
    for (bit_number = 0; bit_number < 4; bit_number++)
    {
        PORTD = (PORTD & 0xF0) | (1 << bit_number);
        _delay_ms(250);
    }
    PORTD &= 0xF0;
}
