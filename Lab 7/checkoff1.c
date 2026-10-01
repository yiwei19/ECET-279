/*
 * Lab 7 Check Off 1
 *
 * I/O table:
 *   Potentiometer 1 wiper -> PF1 / ADC1
 *   Eight test LEDs       -> PORTC
 */

#include <avr/io.h>
#include <stdint.h>

#include "ADC.h"
#include "Debugger.h"

void IO_init(void);
void ADC_init(void);

int main(void)
{
    uint16_t adc_value;

    io_init();
    ADC_init();
    initDebug();

    while (1)
    {
        adc_value = ADC_convert(1);
        LED_PORT = adc_value >> 2;
    }
}

void IO_init(void)
{
    // LED as output
    LED_DDR = 0xFF;
    LED_PORT = 0x00;

    // BUTTON as input with pull-up
    BUTTON_DDR &= ~(1 << BUTTON_BIT);

    // Disable pull-up because button is active high
    BUTTON_PORT &= ~(1 << BUTTON_BIT);
}