#include <avr/io.h>
#include "Timer.h"
#include "Debugger.h"

int main() {
	initDebug();
    Timer1_init();
    Timer1_PWM_245(90);

    while(1) {
    }
}
