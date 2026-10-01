#ifndef ADC_H
#define ADC_H

#define LED_DDR  DDRC
#define LED_PORT PORTC

#define BUTTON_DDR  DDRA
#define BUTTON_PORT PORTA
#define BUTTON_PIN  PINA
#define BUTTON_BIT  PA4

#include <stdint.h>

void ADC_init(void);
uint16_t ADC_convert(uint8_t channel);

#endif