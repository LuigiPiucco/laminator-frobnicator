#include "cylinder.hpp"
#include "pc_interrupts.hpp"
#include "serial_logging.hpp"
#include <Arduino.h>
#include <float.h>
#include <stdint.h>

Cylinder::Cylinder(uint8_t motor_pin, uint8_t encoder_pin, uint32_t diameter)
    : motor_pin(motor_pin), encoder_pin(encoder_pin), diameter(diameter) {}

Cylinder::~Cylinder() { pcint_detach(this->encoder_pin); }

void Cylinder::setup() {
  pinMode(this->motor_pin, OUTPUT);
  pinMode(this->encoder_pin, INPUT_PULLUP);
  pcint_attach(this->encoder_pin, (ISRHandler *)&Cylinder::handle_encoder,
               this);
}

double Cylinder::run() {
  uint32_t current_time = millis();
  double ticks_per_moment = (double)this->pulse_count /
                            (double)(current_time - this->last_pulse_check);
  double circumference_length = PI * (double)this->diameter;
  double circumference_per_tick = circumference_length / 24;
  double circumference_per_moment = circumference_per_tick * ticks_per_moment / (60.0*10e3);

  double power_ratio =
      0x17 * max(1.0 - circumference_per_moment / this->speed_setpoint, 0);
  this->current_power = lround(power_ratio);
  analogWrite(this->motor_pin, this->current_power);

  double circumference_spun =
      circumference_per_tick * (double)this->pulse_count;

  this->pulse_count = 0;
  this->last_pulse_check = current_time;

  return circumference_spun;
}
void Cylinder::stop() {
  this->current_power = 0.0;
  analogWrite(this->motor_pin, this->current_power);
}
uint8_t Cylinder::power() const { return this->current_power; }
uint32_t Cylinder::pulses() const { return this->pulse_count; }

void Cylinder::handle_encoder(volatile Cylinder *self) { ++self->pulse_count; }
