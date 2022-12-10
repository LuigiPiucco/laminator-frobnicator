#include "sensors.hpp"
#include "pc_interrupts.hpp"
#include <stdint.h>

Mapping::Mapping() : from{0, 1023}, to{0, 1023} {}
Mapping::Mapping(Range to) : from{0, 1023}, to(to) {}
Mapping::Mapping(int16_t min, int16_t max) : from{0, 1023}, to{min, max} {}
Mapping::Mapping(Range from, Range to) : from(from), to(to) {}

AnalogReadout::AnalogReadout(uint8_t pin, Mapping map) : map(map), pin(pin) {}
int16_t AnalogReadout::read(bool conv) const {
  int16_t readout = analogRead(this->pin);
  if (conv) {
    readout = (int16_t)::map(readout, this->map.from.min, this->map.from.max,
                             this->map.to.min, this->map.to.max);
  }
  return readout;
}

DigitalIO::DigitalIO(uint8_t pin) : pin(pin) {}
DigitalIO::~DigitalIO() { pcint_detach(this->pin); }
void DigitalIO::setup() { pinMode(this->pin, OUTPUT); }
void DigitalIO::setup(bool pullup) {
  pinMode(this->pin, pullup ? INPUT_PULLUP : INPUT);
}
void DigitalIO::attach(ISRHandler *isr, volatile void *argument) {
  pcint_attach(this->pin, isr, argument);
}
bool DigitalIO::read() const { return digitalRead(this->pin); }
void DigitalIO::write(bool value) { digitalWrite(this->pin, value); }
void DigitalIO::set() { digitalWrite(this->pin, HIGH); }
void DigitalIO::clear() { digitalWrite(this->pin, LOW); }
