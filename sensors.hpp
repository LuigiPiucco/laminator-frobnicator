#ifndef SENSORS_H_
#define SENSORS_H_

#include "pc_interrupts.hpp"
#include <Arduino.h>
#include <stdint.h>

struct Mapping {
  struct Range {
    int16_t : 5, min : 11, : 5, max : 11;
  };

  // Contrutor padrão, não altera o valor.
  Mapping();
  // Construtor a partir de 1 `Range` de saída, o de entrada é considerado como
  // o limite da porta ([0, 1023]).
  Mapping(Range to);
  // Construtor a partir de dois inteiros para o `Range` de saída, o de entrada
  // é considerado como o limite da porta ([0, 1023]).
  Mapping(int16_t min, int16_t max);
  // Construtor a partir de 2 `Range`s.
  Mapping(Range from, Range to);

  // Membros que compoem a faixa; `from` é antes da conversão e `to` é depois.
  Range from, to;
};

// Modela uma leitura analógica com mapeamento para um afaixa de saída.
struct AnalogReadout {
  // Construtor que preenche os membros. Caso `map` não seja passado, o padrão
  // é nenhum mapeamento.
  AnalogReadout(uint8_t pin, Mapping map = Mapping{});

  int16_t read(bool conv = true) const;

  Mapping map;

  const uint8_t pin;
};

struct DigitalIO {
  DigitalIO(uint8_t pin);
  ~DigitalIO();

  // Configura o modo do pino como saída.
  void setup();
  // Configura o modo do pino como entrada, se `pullup` habilita o resistor
  // interno.
  void setup(bool pullup);

  // Acopla uma interrupção PC no pino.
  void attach(ISRHandler *isr, volatile void *argument);

  // Retorna o valor do pino
  bool read() const;

  // Escreve `value` no pino.
  void write(bool value);
  // Escreve `HIGH` no pino.
  void set();
  // Escreve `LOW` no pino.
  void clear();

  // Valor imutável do pino controlado.
  const uint8_t pin;
};

#endif // SENSORS_H_
