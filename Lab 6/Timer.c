#include "Timer.h"

void Timer0_init(void) {
    TCCR0A = (1 << WGM01); // CTC mode
    TCCR0B = (1 << CS01)|(1 << CS00); // Prescaler = 64

    //1ms delay
    OCR0A = 249;
    TCNT0 = 0;
}

void delay_1ms(uint16_t delay) {
    for (uint16_t i = 0; i < delay; i++) {
        
        /// Clear old compare flag
        TIFR0 = (1 << OCF0A);

        while (!(TIFR0 & (1 << OCF0A))) {
        } // wait until 1ms has passed
    }
}

void Timer1_init(void) {
    // OC1A as output, according to the datasheet, it is PB5
    DDRB |= (1 << PB5);

    // 9-bit Phase Correct PWM
    // WGM13:0 = 0010
    // Non-inverting output on OC1A
    TCCR1A = (1 << COM1A1) | (1 << WGM11);

    // Prescaler = 64
    TCCR1B = (1 << CS11)|(1 << CS10);

    // Initial duty cycle = 10%
    OCR1A = 51;
    TCNT1 = 0;

}

void Timer1_PWM_245(uint8_t Duty_Cycle) {
    OCR1A = ((uint32_t)Duty_Cycle * 511) / 100;
}