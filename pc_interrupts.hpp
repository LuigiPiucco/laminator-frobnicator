#ifndef PC_INTERRUPTS_H_
#define PC_INTERRUPTS_H_

#include <stdint.h>

typedef void (*ISRHandler)(volatile void *);

void pcint_attach(uint8_t pin, ISRHandler isr, volatile void *argument);

void pcint_detach(uint8_t pin);

#endif // PC_INTERRUPTS_H_
