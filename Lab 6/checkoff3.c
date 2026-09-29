#include "Timer.h"
#include <avr/io.h>

#define START_BUTTON PA4
#define STOP_BUTTON PA5

void io_init(void);

int main() {
    io_init();
    Timer1_init();
    
    while(1) {
        // START button pushed
        if (!(PINA & (1 << START_BUTTON))) {
            // Enable OC1A PWM output
            TCCR1A |= (1 << COM1A1);
            ramp_up_delay_n_steps(15, 95, 5000, 8);

            // wait it release
            while(!(PINA & (1 << START_BUTTON))) {
            }
        }

        // STOP button
        if(!(PINA & (1 << STOP_BUTTON))) {
            // Disable PWM output
            TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));
            while(!(PINA & (1 << STOP_BUTTON))) {
            }
        }
    }
}

void io_init(void) {
    // PA4, PA5 as input
    DDRA &= ~((1 << START_BUTTON) |
              (1 << STOP_BUTTON));
}