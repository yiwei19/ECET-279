/*
 * Project: ECET 279 Labs 4-5 Washing Machine
 * Author: Ivy Yi Wei
 * Date: 09/27/2026
 *
 * Description:
 * Controls a complete wash cycle: fill, agitate, drain, rinse,
 * drain, spin, and done. The machine waits for the door to close
 * before starting and requires the door to open after finishing.
 *
 * Hardware:
 * PA0 Hot, PA1 Warm, PA2 Cold, PA3 Door Open, PA4 Start
 * PC0 Drain, PC1 Hot Valve, PC2 Cold Valve
 * PC3 Done LED, PC4 Agitate LED, PC5 Spin LED
 * PD0-PD3 Stepper motor driver IN1-IN4
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "stepper.h"
// #include "debugger.h"

#define HOT_BIT       PA0
#define WARM_BIT      PA1
#define COLD_BIT      PA2
#define DOOR_BIT      PA3
#define START_BIT     PA4
#define INPUT_MASK    0x1F

#define DRAIN_BIT     PC0
#define HOT_VALVE     PC1
#define COLD_VALVE    PC2
#define DONE_LED      PC3
#define AGITATE_LED   PC4
#define SPIN_LED      PC5
#define OUTPUT_MASK   0x3F

static void io_init(void);

int main(void)
{
    io_init();
    // initDebug();

    while (1)
    {
        /* Wait for Start push-button. */
        while (PINA & (1 << START_BIT))
        {
        }
        
        /* Wait for Door to close*/
        while (PINA & (1 << DOOR_BIT))
        {
        }
        
        /* Read temperature settings */
        if (!(PINA & (1 << HOT_BIT)))
        {
            PORTC |= (1 << HOT_VALVE);
        }
        else if (!(PINA & (1 << WARM_BIT)))
        {
            PORTC |= (1 << HOT_VALVE);
            PORTC |= (1 << COLD_VALVE);

        }
        else if (!(PINA & (1 << COLD_BIT)))
        {
            PORTC |= (1 << COLD_VALVE);
        }
        _delay_ms(2000);

        // values OFF
        PORTC &= ~(1 << HOT_VALVE);
        PORTC &= ~(1 << COLD_VALVE);

        /* AGITATE for 8 seconds */
        PORTC |= (1 << AGITATE_LED);
        motor_control(AGITATE, 8);
        _delay_ms(8000);
        PORTC &= ~(1 << AGITATE_LED);
        motor_off();

        /* DRAIN for 1 second and Drain Value off*/
        PORTC |= (1 << DRAIN_BIT);
        _delay_ms(1000);
        PORTC &= ~(1 << DRAIN_BIT);


        /* Read temperature settings */
        if (!(PINA & (1 << HOT_BIT)))
        {
            PORTC |= (1 << HOT_VALVE);
        }
        else if (!(PINA & (1 << WARM_BIT)))
        {
            PORTC |= (1 << HOT_VALVE);
            PORTC |= (1 << COLD_VALVE);

        }
        else if (!(PINA & (1 << COLD_BIT)))
        {
            PORTC |= (1 << COLD_VALVE);
        }
        _delay_ms(2000);

        // values OFF
        PORTC &= ~(1 << HOT_VALVE);
        PORTC &= ~(1 << COLD_VALVE);

        /* AGITATE for 4 seconds */
        PORTC |= (1 << AGITATE_LED);
        motor_control(AGITATE, 4);
        _delay_ms(4000);
        PORTC &= ~(1 << AGITATE_LED);
        motor_off();

        /* DRAIN for 1 second and Drain Value off*/
        PORTC |= (1 << DRAIN_BIT);
        _delay_ms(1000);
        PORTC &= ~(1 << DRAIN_BIT);

        /* SPIN for 4 seconds */
        PORTC |= (1 << SPIN_LED);
        motor_control(SPIN, 4);
        _delay_ms(4000);
        PORTC &= ~(1 << SPIN_LED);
        motor_off();

        // all values OFF
        PORTC &= ~(1 << HOT_VALVE);
        PORTC &= ~(1 << COLD_VALVE);
        PORTC &= ~(1 << DRAIN_BIT);

        // Done LED ON
        PORTC |= (1 << DONE_LED);

        // Wait for Door to open
        while (PINA & (1 << DOOR_BIT))
        {
        }
        // Done LED OFF
        PORTC &= ~(1 << DONE_LED);

    }
}

/*
 * Initialize the I/O ports.
 */
void io_init(void)
{
    DDRA &= ~INPUT_MASK;
    PORTA |= INPUT_MASK;

    DDRC |= OUTPUT_MASK;
    PORTC &= ~OUTPUT_MASK;

    stepper_init();
}