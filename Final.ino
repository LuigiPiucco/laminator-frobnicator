#include "cylinder.hpp"
#include "pins.hpp"
#include "serial_logging.hpp"

Cylinder laminator_cyl =
    Cylinder(laminator_motor, laminator_encoder, laminator_cylinder_diameter);
Cylinder extractor_cyl =
    Cylinder(extractor_motor, extractor_encoder, extractor_cylinder_diameter);

void setup() {
  serial_begin(9600);

  laminator_cyl.setup();
  extractor_cyl.setup();

  laminator_cyl.speed_setpoint = 200;
  extractor_cyl.speed_setpoint = 200;
}

void loop() {
  laminator_cyl.run();
  extractor_cyl.run();
}
