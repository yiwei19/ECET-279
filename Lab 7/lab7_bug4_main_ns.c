#include <avr/io.h>
#include "lab7_bug4_adc_h.h"

#define BUTTON_PRESSED   (PINA & (1 << PA0))

static uint8_t next_channel(uint8_t ch);

int main(void)
{
    DDRC = 0xFF;
    PORTC = 0x00;

    DDRA &= ~(1 << DDA0);
    PORTA |= (1 << PA0);

    Init_ADC();

    uint8_t ch = 5;

    while (1)
    {
        if (BUTTON_PRESSED)
        {
            while (BUTTON_PRESSED) { ; }
            ch = next_channel(ch);
        }

        uint16_t val = ADC_read10(ch);
        PORTC = (uint8_t)(val >> 2);
    }
}

static uint8_t next_channel(uint8_t ch)
{
    if (ch < 5) return 5;
    if (ch >= 7) return 5;
    return (uint8_t)(ch + 1);
}
