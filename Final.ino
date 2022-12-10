#include "cylinder.hpp"
#include "pc_interrupts.hpp"
#include "pins.hpp"
#include "sensors.hpp"
#include "serial_logging.hpp"

Cylinder laminator_cyl =
    Cylinder(laminator_motor, laminator_encoder, laminator_cylinder_diameter);
Cylinder extractor_cyl =
    Cylinder(extractor_motor, extractor_encoder, extractor_cylinder_diameter);
AnalogReadout speed_pot = AnalogReadout{speed_potentiometer, {0, 80}};
DigitalIO course_ender = DigitalIO{course_ender_sensor};
DigitalIO auto_mode = DigitalIO{auto_mode_sensor};
DigitalIO pedal = DigitalIO{pedal_sensor};
DigitalIO on_indicator = DigitalIO{on_led};
DigitalIO problem_indicator = DigitalIO{problem_led};
DigitalIO paper_indicator = DigitalIO{paper_led};
DigitalIO paper_detector = DigitalIO{paper_sensor};
DigitalIO reset_quant = DigitalIO{reset_quantity_btn};

void stall_while_blocked(volatile void *) {
  while (!course_ender.read()) {
    on_indicator.clear();
    problem_indicator.set();
    laminator_cyl.stop();
    extractor_cyl.stop();
  }
}


void setup() {
  PCMSK0 = 0;
  PCMSK1 = 0;
  PCMSK2 = 0;

  serial_begin(9600);
  course_ender.setup(true);
  course_ender.attach((ISRHandler *)stall_while_blocked, NULL);

  auto_mode.setup(true);
  pedal.setup(true);

  laminator_cyl.setup();
  extractor_cyl.setup();

  laminator_cyl.speed_setpoint = 200;
  extractor_cyl.speed_setpoint = 200;
  on_indicator.setup();
  problem_indicator.setup();
  paper_indicator.setup();

  paper_detector.setup(true);

  reset_quant.setup(true);
}

void loop() {
  if (!reset_quant.read()) {
    paper_length = 0.0;
  }

  int16_t desired_speed = speed_pot.read();

  if (!paper_detector.read() && (!auto_mode.read() || !pedal.read())) {
    laminator_cyl.speed_setpoint = desired_speed;
    double laminator_spun = laminator_cyl.run();
    extractor_cyl.speed_setpoint = desired_speed;
    double extractor_spun = extractor_cyl.run();
  } else {
    laminator_cyl.stop();
    extractor_cyl.stop();
  }

  on_indicator.set();
  problem_indicator.clear();
  paper_indicator.write(paper_detector.read());
}
