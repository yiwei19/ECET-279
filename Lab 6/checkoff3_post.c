#include <avr/io.h>
#include "Timer.h"
#include "Debugger.h"

#define START_BUTTON PA4
#define STOP_BUTTON PA5

void io_init(void);
/*void ramp_up_delay_n_steps(uint8_t start, uint8_t end, uint16_t ms_time, uint8_t num_steps);*/
int main() {
    io_init();
	
	Timer0_init();
    Timer1_init();
	
	initDebug();
	
    while(1) {
        // START button pushed
        if (PINA & (1 << START_BUTTON)) {
            // Enable OC1A PWM output
            TCCR1A |= (1 << COM1A1);
			
            ramp_up_delay_n_steps(0, 90, 9000,8);

//             // wait it release
//             while((PINA & (1 << START_BUTTON))) {
//             }

        }
// 
        // STOP button
        if(PINA & (1 << STOP_BUTTON)) {
            // Disable PWM output
			PORTC ^= 0xFF;
            TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));
//             while(!(PINA & (1 << STOP_BUTTON))) {
//             }
        }
    }
}

void io_init(void) {
    // PA4, PA5 as input
    DDRA &= ~((1 << START_BUTTON) |
              (1 << STOP_BUTTON));
	PORTA |= (1 << START_BUTTON) |
			 (1 << STOP_BUTTON);
	DDRC = 0xFF;
	PORTC = 0x00;
}
