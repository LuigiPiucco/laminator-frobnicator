#ifndef PINS_H_
#define PINS_H_

// Atribuições de pinos para cada periférico.

constexpr uint8_t speed_potentiometer = A0;

constexpr uint8_t auto_mode_sensor = 8;
constexpr uint8_t pedal_sensor = 3;

constexpr uint8_t laminator_encoder = 7;
constexpr uint8_t extractor_encoder = 6;

constexpr uint8_t reset_quantity_btn = 2;
constexpr uint8_t course_ender_sensor = 4;

constexpr uint8_t paper_sensor = 5;

constexpr uint8_t laminator_motor = 10;
constexpr uint8_t extractor_motor = 9;

constexpr uint8_t on_led = 11;
constexpr uint8_t problem_led = 12;
constexpr uint8_t paper_led = 13;

#endif // PINS_H_
