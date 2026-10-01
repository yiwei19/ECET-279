#include "ADC.h"

#include <avr/io.h>

void ADC_init(void)
{
    // ADC1 = PF1, ADC2 = PF2, ADC3 = PF3
    // Configure PF1, PF2, PF3 as inputs
    DDRF &= ~((1 << DDF1) |
              (1 << DDF2) |
              (1 << DDF3));

    // Disable internal pull-ups
    PORTF &= ~((1 << PORTF1) |
               (1 << PORTF2) |
               (1 << PORTF3));

    // Disable digital input buffers on ADC1, ADC2, ADC3
    DIDR0 |= (1 << ADC1D) |
             (1 << ADC2D) |
             (1 << ADC3D);

    // AVCC reference
    // Right-adjusted result (ADLAR = 0)
    // Start with ADC1 selected
    ADMUX = (1 << REFS0) |
            (1 << MUX0);

    // MUX5 = 0
    // No auto trigger
    ADCSRB = 0;

    // Enable ADC
    // Prescaler = 128
    ADCSRA = (1 << ADEN)  |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
}

uint16_t ADC_convert(uint8_t channel)
{
    // 1. Select ADC channel
    ADMUX = (ADMUX & 0xE0) | channel;

    // 2. Start ADC conversion
    ADCSRA |= (1 << ADSC);

    // 3. Wait until conversion is complete
    while (!(ADCSRA & (1 << ADIF)))
    {
        // wait
    }

    // 4. Clear ADIF
    ADCSRA |= (1 << ADIF);

    // 5. Return the 10-bit ADC result
    return ADC;
}