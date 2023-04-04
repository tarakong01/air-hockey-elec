#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "constants.h"

void set_up_display()
{
//   setting up the display
//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0, 0);
//   display.display();
}

void init_pins()
{
    pinMode(PWM, OUTPUT);
}

void setup()
{
    // put your setup code here, to run once:
    init_pins();
    set_up_display();
}

void loop()
{
    // put your main code here, to run repeatedly:
    pwm_start(PWM, DC_FREQ, speed, RESOLUTION_12B_COMPARE_FORMAT);
}