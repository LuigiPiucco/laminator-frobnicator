#include "TaskTypes.h"
#include "cylinder.hpp"
#include "pc_interrupts.hpp"
#include "pins.hpp"
#include "sensors.hpp"
#include "serial_logging.hpp"
#include <IoAbstractionWire.h>
#include <LiquidCrystalIO.h>
#include <Wire.h>

Cylinder laminator_cyl =
    Cylinder{laminator_motor, laminator_encoder, laminator_cylinder_diameter};
Cylinder extractor_cyl =
    Cylinder{extractor_motor, extractor_encoder, extractor_cylinder_diameter};
AnalogReadout speed_pot = AnalogReadout{speed_potentiometer, {0, 80}};
DigitalIO course_ender = DigitalIO{course_ender_sensor};
DigitalIO auto_mode = DigitalIO{auto_mode_sensor};
DigitalIO pedal = DigitalIO{pedal_sensor};
DigitalIO on_indicator = DigitalIO{on_led};
DigitalIO problem_indicator = DigitalIO{problem_led};
DigitalIO paper_indicator = DigitalIO{paper_led};
DigitalIO paper_detector = DigitalIO{paper_sensor};
DigitalIO reset_quant = DigitalIO{reset_quantity_btn};
LiquidCrystalI2C_RS_EN(lcd, 0x27, false);
volatile double paper_length = 0.0;

void stall_while_blocked(volatile void *) {
  while (!course_ender.read()) {
    on_indicator.clear();
    problem_indicator.set();
    laminator_cyl.stop();
    extractor_cyl.stop();
  }
}

void repaint() {
  lcd.setCursor(0, 0);
  char vel_msg[17];
  snprintf(vel_msg, 17, "Vel.: %5d mm/s", speed_pot.read());
  lcd.print(vel_msg);
  lcd.setCursor(0, 1);
  char float_msg[6];
  dtostrf(paper_length / 1000.0, 5, 2, float_msg);
  char done_msg[17];
  snprintf(done_msg, 17, "Pronto: %s m", float_msg);
  lcd.print(done_msg);
}

void setup() {
  PCMSK0 = 0;
  PCMSK1 = 0;
  PCMSK2 = 0;

  serial_begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.configureBacklightPin(3, LiquidCrystal::BACKLIGHT_NORMAL);
  lcd.backlight();

  course_ender.setup(true);
  course_ender.attach((ISRHandler *)stall_while_blocked, NULL);

  auto_mode.setup(true);
  pedal.setup(true);

  laminator_cyl.setup();
  extractor_cyl.setup();

  taskManager.scheduleFixedRate(16666, repaint, TIME_MICROS);

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

  taskManager.runLoop();

  int16_t desired_speed = speed_pot.read();

  if (!paper_detector.read() && !(auto_mode.read() && pedal.read())) {
    laminator_cyl.speed_setpoint = desired_speed;
    double laminator_spun = laminator_cyl.run();
    extractor_cyl.speed_setpoint = desired_speed;
    double extractor_spun = extractor_cyl.run();
    if (!isnan(laminator_spun) && !isnan(extractor_spun)) {
      paper_length += (laminator_spun + extractor_spun) / 2.0;
    }
  } else {
    laminator_cyl.stop();
    extractor_cyl.stop();
  }

  on_indicator.set();
  problem_indicator.clear();
  paper_indicator.write(paper_detector.read());
}
