#include <avr/io.h>
#include "Timer.h"

int main() {
    Timer1_init();
    Timer1_PWM_245(50);

    while(1) {
    }
}
