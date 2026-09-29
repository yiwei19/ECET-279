
#include "timer1_oc1a_7820.h"
void Timer1_OC1A_7820_init(void)
{
    DDRB |= (1 << PB5);
    TCCR1A = 0; 
	TCCR1B = 0;
    TCCR1A |= (1 << WGM11) | (1 << WGM10);
    TCCR1B |= (1 << WGM12);
    TCCR1A |= (1 << COM1A1);
    TCCR1B |= (1 << CS10);
    OCR1A = (uint16_t)((uint32_t)30 * 1023UL / 100UL);
}
void Timer1_OC1A_setDuty(uint8_t d)
{
    if (d > 100)
    {
        d = 100;
    }
    OCR1A = (uint16_t)((uint32_t)d * 1023UL / 100UL);
}
