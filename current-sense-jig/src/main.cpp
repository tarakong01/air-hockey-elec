#include <Arduino.h>
#include "constants.h"

void init_pins() {
  pinMode(PWM, OUTPUT);
}

void setup() {
  // put your setup code here, to run once:
    init_pins();
}

void loop() {
  // put your main code here, to run repeatedly:
  pwm_start(PWM, 48, SPEED, RESOLUTION_12B_COMPARE_FORMAT);
}