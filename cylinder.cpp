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
  pcint_attach(this->encoder_pin, (ISRHandler)&Cylinder::handle_encoder, this);
}

void Cylinder::run() {
  uint64_t current_time = millis();
  double ticks_per_moment = (double)this->pulse_count /
                            (double)(current_time - this->last_pulse_check);
  double circumference_length = PI * (double)this->diameter;
  double circumference_per_tick = circumference_length / 24;
  double circumference_per_moment = ticks_per_moment * circumference_per_tick;

  double power_ratio =
      max(1 - circumference_per_moment / this->speed_setpoint, 0);
  this->current_power = lround(0xFF * power_ratio);
  serial_println(this->current_power, DEC);
  analogWrite(this->motor_pin, this->current_power);

  this->pulse_count = 0;
  this->last_pulse_check = current_time;
}

void Cylinder::stop() {
  this->current_power = 0;
  analogWrite(this->motor_pin, this->current_power);
}

uint8_t Cylinder::power() const { return this->current_power; }

void Cylinder::handle_encoder(Cylinder *volatile self) { self->count_pulse(); }

void Cylinder::count_pulse() { ++this->pulse_count; }