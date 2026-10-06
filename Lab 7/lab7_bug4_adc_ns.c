#include "lab7_bug4_adc_h.h"

void Init_ADC(void)
{
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRB = 0x00;
    DDRF &= ~((1 << PF5) | (1 << PF6) | (1 << PF7));
    PORTF &= ~((1 << PF5) | (1 << PF6) | (1 << PF7));
}

uint16_t ADC_read10(uint8_t chan)
{
    uint16_t v;
    ADMUX &= ~(1 << ADLAR);
    ADMUX = (ADMUX & 0xE0) | chan;
    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADSC)) == 0) { ; }
    v = ADCL;
    v |= ((uint16_t)ADCH << 8);
    return v;
}
