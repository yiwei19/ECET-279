/*
 * Project name: Wash_MC_Sys_Dev
 * File name: Control_final.c
 * Author: Ivy Yi Wei
 * Date: September 15, 2026
 *
 * Description:
 * This program controls a complete model washing-machine cycle. After Start is
 * pressed and the door is closed, it performs wash fill, agitation, drain,
 * rinse fill, agitation, drain, and spin. It then lights the Done LED and
 * requires the door to be opened before another cycle can start.
 *
 * ATmega2560 peripherals and hardware connections:
 * PA0 - Hot temperature switch (active low)
 * PA1 - Warm temperature switch (active low)
 * PA2 - Cold temperature switch (active low)
 * PA3 - Door-open switch (active low when the door is open)
 * PA4 - Start pushbutton (active low)
 * PD0 - ULN2003 IN1
 * PD1 - ULN2003 IN2
 * PD2 - ULN2003 IN3
 * PD3 - ULN2003 IN4
 * PC0 - Drain valve
 * PC1 - Hot-water valve
 * PC2 - Cold-water valve
 * PC3 - Wash-done LED
 * PC4 - Agitate LED
 * PC5 - Spin LED
 * Timer 0 / util/delay - motor inter-step and valve timing delays
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "stepper.h"

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

#define HOT_SELECTION        (1 << HOT_SELECT_BIT)
#define WARM_SELECTION       (1 << WARM_SELECT_BIT)
#define COLD_SELECTION       (1 << COLD_SELECT_BIT)
#define TEMPERATURE_MASK     (HOT_SELECTION | WARM_SELECTION | COLD_SELECTION)

static void io_init(void);
static uint8_t input_is_active(uint8_t bit_number);
static uint8_t read_temperature_selection(void);
static void wait_for_start(void);
static void wait_for_door_closed(void);
static void fill_for_two_seconds(void);
static void drain_for_one_second(void);
static void run_motor_with_indicator(char mode, uint8_t seconds);
static void delay_seconds(uint8_t seconds);
static void all_outputs_off(void);

int main(void)
{
    io_init();

#if defined(USE_DEBUGGER)
    initDebug();
#endif

    while (1)
    {
        wait_for_start();
        wait_for_door_closed();

        fill_for_two_seconds();
        run_motor_with_indicator(MOTOR_AGITATE, 8);
        drain_for_one_second();

        fill_for_two_seconds();
        run_motor_with_indicator(MOTOR_AGITATE, 4);
        drain_for_one_second();

        run_motor_with_indicator(MOTOR_SPIN, 4);

        all_outputs_off();
        PORTC |= (1 << DONE_LED_BIT);

        /* Do not permit another cycle until the door has been opened. */
        while (!input_is_active(DOOR_OPEN_BIT))
        {
        }

        PORTC &= (uint8_t)~(1 << DONE_LED_BIT);
    }
}

static void io_init(void)
{
    DDRA &= (uint8_t)~INPUT_MASK;
    PORTA |= INPUT_MASK;

    DDRC |= CONTROL_OUTPUT_MASK;
    PORTC &= (uint8_t)~CONTROL_OUTPUT_MASK;

    stepper_init();
}

static uint8_t input_is_active(uint8_t bit_number)
{
    return (PINA & (1 << bit_number)) == 0;
}

static uint8_t read_temperature_selection(void)
{
    uint8_t active_inputs;

    while (1)
    {
        active_inputs = (uint8_t)(~PINA) & TEMPERATURE_MASK;

        if ((active_inputs == HOT_SELECTION) ||
            (active_inputs == WARM_SELECTION) ||
            (active_inputs == COLD_SELECTION))
        {
            return active_inputs;
        }
    }
}

static void wait_for_start(void)
{
    while (!input_is_active(START_BUTTON_BIT))
    {
    }

    _delay_ms(20);

    while (input_is_active(START_BUTTON_BIT))
    {
    }
}

static void wait_for_door_closed(void)
{
    while (input_is_active(DOOR_OPEN_BIT))
    {
    }
}

static void fill_for_two_seconds(void)
{
    uint8_t selection = read_temperature_selection();

    PORTC &= (uint8_t)~((1 << HOT_VALVE_BIT) | (1 << COLD_VALVE_BIT));

    if (selection == HOT_SELECTION)
    {
        PORTC |= (1 << HOT_VALVE_BIT);
    }
    else if (selection == WARM_SELECTION)
    {
        PORTC |= (1 << HOT_VALVE_BIT) | (1 << COLD_VALVE_BIT);
    }
    else
    {
        PORTC |= (1 << COLD_VALVE_BIT);
    }

    delay_seconds(2);
    PORTC &= (uint8_t)~((1 << HOT_VALVE_BIT) | (1 << COLD_VALVE_BIT));
}

static void drain_for_one_second(void)
{
    PORTC |= (1 << DRAIN_VALVE_BIT);
    delay_seconds(1);
    PORTC &= (uint8_t)~(1 << DRAIN_VALVE_BIT);
}

static void run_motor_with_indicator(char mode, uint8_t seconds)
{
    if (mode == MOTOR_AGITATE)
    {
        PORTC |= (1 << AGITATE_LED_BIT);
        motor_run(mode, seconds);
        PORTC &= (uint8_t)~(1 << AGITATE_LED_BIT);
    }
    else if (mode == MOTOR_SPIN)
    {
        PORTC |= (1 << SPIN_LED_BIT);
        motor_run(mode, seconds);
        PORTC &= (uint8_t)~(1 << SPIN_LED_BIT);
    }
    else
    {
        motor_off();
    }
}

static void delay_seconds(uint8_t seconds)
{
    uint8_t second;
    uint16_t millisecond;

    for (second = 0; second < seconds; second++)
    {
        for (millisecond = 0; millisecond < 1000; millisecond++)
        {
            _delay_ms(1);
        }
    }
}

static void all_outputs_off(void)
{
    PORTC &= (uint8_t)~CONTROL_OUTPUT_MASK;
    motor_off();
}
