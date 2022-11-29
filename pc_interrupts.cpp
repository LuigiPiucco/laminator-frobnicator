#include "pc_interrupts.hpp"
#include "serial_logging.hpp"
#include <Arduino.h>
#include <stdint.h>

// ---
// Definições auxiliares.

// Macro para facilitar a escrita de identificadores em outros macros.
#define PCINT_IDENT(num, suffix) PCINT##num##_##suffix

// Mapa de (índice PCINT) para (endereço de PORT) correspondente.
static volatile uint8_t *const PCINT_ports[3] = {&PINB, &PINC, &PIND};

// ---
// Estados prévios dos pinos de cada porta. Serve para comparar com o atual e
// descobrir quais pinos realmente mudaram (lembre que a interrupção é para
// qualquer mudança na porta, cabe à rotina saber qual foi o pino).

// Estado prévio da PORTB.
static volatile uint8_t PCINT0_last_state = *PCINT_ports[0];
// Estado prévio da PORTC.
static volatile uint8_t PCINT1_last_state = *PCINT_ports[1];
// Estado prévio da PORTD.
static volatile uint8_t PCINT2_last_state = *PCINT_ports[2];

// ---
// Listas de rotinas PC por pino. Ou seja, cada pino pode ter uma função
// diferente acoplada. Essas coisas são listas de ponteiros para função.
//
// Logo abaixo há a lista de pares parâmetro-rotina, o valor de cada qual sendo
// passado à rotina em sua invocação.

// Callbacks da PORTB.
static ISRHandler PCINT0_interrupt_vector[8] = {0};
// Callbacks da PORTC.
static ISRHandler PCINT1_interrupt_vector[8] = {0};
// Callbacks da PORTD.
static ISRHandler PCINT2_interrupt_vector[8] = {0};
// Argumentos da PORTB.
static volatile void *PCINT0_argument_vector[8] = {0};
// Arguments da PORTC.
static volatile void *PCINT1_argument_vector[8] = {0};
// Argumentos da PORTD.
static volatile void *PCINT2_argument_vector[8] = {0};

// ---
// Definição das rotinas de serviço principais.

// Macro com o corpo padrão das rotinas PC. É parametrizado com o número da
// interrupção a que corresponde. É esse quem faz o corpo passado ao macro ISR,
// sua responsabilidade é detectar quais pinos mudaram e chamar o callback (das
// listas acima) correspondente.
#define PCISR_BODY(num)                                                        \
  {                                                                            \
    uint8_t current_pins = *PCINT_ports[num];                                  \
    uint8_t changed_pins = PCINT_IDENT(num, last_state) ^ current_pins;        \
    for (uint8_t i = 0; i < 8; ++i) {                                          \
      if (changed_pins & (1 << i)) {                                           \
        PCINT_IDENT(num, interrupt_vector)                                     \
        [i](PCINT_IDENT(num, argument_vector)[i]);                             \
      }                                                                        \
    }                                                                          \
    PCINT_IDENT(num, last_state) = current_pins;                               \
  }

// Rotina PC principal para a PORTB.
ISR(PCINT0_vect) PCISR_BODY(0);

// Rotina PC principal para a PORTC.
ISR(PCINT1_vect) PCISR_BODY(1);

// Rotina PC principal para a PORTD.
ISR(PCINT2_vect) PCISR_BODY(2);

// ---
// Macros liga/desliga para PCINT.

// Ativa o estado das interrupções PCint.
#define SET_PCINT(int) (PCICR |= 1 << (int))
// Ativa o estado das interrupções PCint, para o pino pin.
#define SET_PCINT_PIN(int, pin) ((PCMSK##int |= 1 << (pin)))
// Desliga o estado das interrupções PCint.
#define CLEAR_PCINT(int) (PCICR &= ~(1 << (int)))
// Desliga o estado das interrupções PCint, para o pino pin.
#define CLEAR_PCINT_PIN(int, pin) ((PCMSK##int &= ~(1 << (pin))))

// ---
// Definição da API pública de interrupções.

// Acopla a rotina isr à interrupção PC sob o pino pin.
void pcint_attach(uint8_t pin, ISRHandler isr, volatile void *argument) {
  volatile uint8_t *pcint = portInputRegister(digitalPinToPort(pin));

  if (pcint == &PINB) {
    if (PCINT_IDENT(0, interrupt_vector)[pin] == nullptr &&
        PCINT_IDENT(0, argument_vector)[pin] == nullptr) {
      PCINT_IDENT(0, interrupt_vector)[pin] = isr;
      PCINT_IDENT(0, argument_vector)[pin] = argument;
      SET_PCINT_PIN(0, pin);
      SET_PCINT(0);
    }
  } else if (pcint == &PINC) {
    if (PCINT_IDENT(1, interrupt_vector)[pin] == nullptr &&
        PCINT_IDENT(1, argument_vector)[pin] == nullptr) {
      PCINT_IDENT(1, interrupt_vector)[pin] = isr;
      PCINT_IDENT(1, argument_vector)[pin] = argument;
      SET_PCINT_PIN(1, pin);
      SET_PCINT(1);
    }
  }
  if (pcint == &PIND) {
    if (PCINT_IDENT(2, interrupt_vector)[pin] == nullptr &&
        PCINT_IDENT(2, argument_vector)[pin] == nullptr) {
      PCINT_IDENT(2, interrupt_vector)[pin] = isr;
      PCINT_IDENT(2, argument_vector)[pin] = argument;
      SET_PCINT_PIN(2, pin);
      SET_PCINT(2);
    }
  } else {
    serial_print("Erro: tentando acoplar interrupção à porta inexistente ");
    serial_print((uintptr_t)pcint, DEC);
    serial_println("!");
  }
}

// Desacopla rotina da interrupção PC interrupt sob o pino pin.
void pcint_detach(uint8_t pin) {
  uintptr_t pcint = portOutputRegister(digitalPinToPort(pin)) - &PORTB;

  switch (pcint) {
  case 0:
    PCINT_IDENT(0, interrupt_vector)[pin] = nullptr;
    PCINT_IDENT(0, argument_vector)[pin] = nullptr;
    CLEAR_PCINT_PIN(0, pin);
    SET_PCINT(0);
    if (!PCMSK0) {
      CLEAR_PCINT(0);
    }
    break;
  case 1:
    PCINT_IDENT(1, interrupt_vector)[pin] = nullptr;
    PCINT_IDENT(1, argument_vector)[pin] = nullptr;
    CLEAR_PCINT_PIN(1, pin);
    SET_PCINT(1);
    if (!PCMSK1) {
      CLEAR_PCINT(1);
    }
    break;
  case 2:
    PCINT_IDENT(2, interrupt_vector)[pin] = nullptr;
    PCINT_IDENT(2, argument_vector)[pin] = nullptr;
    CLEAR_PCINT_PIN(2, pin);
    if (!PCMSK2) {
      CLEAR_PCINT(2);
    }
    break;
  default:
    serial_print("Erro: tentando desacoplar interrupção da porta inexistente ");
    serial_print(pcint, DEC);
    serial_println("!");
  }
}
