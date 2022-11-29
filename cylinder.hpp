#ifndef CYLINDER_H_
#define CYLINDER_H_

#include <stdint.h>

constexpr uint32_t laminator_cylinder_diameter = 100 /* mm */;
constexpr uint32_t extractor_cylinder_diameter = 50 /* mm */;

class Cylinder {
public:
  Cylinder(uint8_t motor_pin, uint8_t encoder_pin, uint32_t diameter);
  ~Cylinder();

  void setup();

  volatile uint16_t speed_setpoint = 0;

  void run();
  void stop();

  uint8_t power() const;

private:
  const uint8_t motor_pin;
  const uint8_t encoder_pin;

  const uint32_t diameter;
  uint32_t pulse_count = 0;
  uint64_t last_pulse_check = 0;

  uint8_t current_power = 0;

  static void handle_encoder(Cylinder *volatile self);
  void count_pulse();
};

#endif // CYLINDER_H_
