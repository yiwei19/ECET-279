#include <avr/io.h>
#include "Timer.h"

#define START_BUTTON PA4
#define STOP_BUTTON PA5
#define TEST_BUTTON PA6

#define TEST_LED PC0 // test/mode LED

void io_init(void);
void Timer0_init(void);
void delay_1ms(uint16_t delay);

int main() {
    io_init();
    Timer0_init();

    while(1) {
        // turn the LED ON
        PORTC |= (1 << TEST_LED);
        // delay 1 second
        delay_1ms(1000);

        // turn the LED OFF
        PORTC &= ~(1 << TEST_LED);
        // delay 1 second
        delay_1ms(1000);
    }
}

void io_init(void) {
    // PA4, PA5, PA6 as input
    DDRA &= ~((1 << START_BUTTON) |
              (1 << STOP_BUTTON) |
              (1 << TEST_BUTTON));
    
    // PC0 as output
    DDRC |= (1 << TEST_LED);
}
