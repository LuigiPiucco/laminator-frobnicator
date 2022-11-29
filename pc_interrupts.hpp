#ifndef PC_INTERRUPTS_H_
#define PC_INTERRUPTS_H_

#include <stdint.h>

// ---
// Definição da API pública de interrupções.

// Tipo das rotinas de interrupção PC. É um ponteiro para função, sem retorno e
// com um parâmetro do tipo ponteiro genérico e volátil.
typedef void (*ISRHandler)(volatile void *);

// Acopla a rotina isr à interrupção PC sob o pino pin.
void pcint_attach(uint8_t pin, ISRHandler isr, volatile void *argument);

// Desacopla rotina da interrupção PC interrupt sob o pino pin.
void pcint_detach(uint8_t pin);

#endif // PC_INTERRUPTS_H_
