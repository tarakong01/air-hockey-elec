#pragma once

#define PWM PA_0 // named like this for PWM command (diff data type)
#define DIR PA1
#define V_SENSE PA2
#define CSA_GND PA3
#define EXT_DUTY_CYCLE PA5
#define ENC_1 PA6
#define ENC_2 PA7

const int PWM_FREQ_HZ = 10000;
// const int DUTY_CYCLE = 50;
const int DUTY_CYCLE_CONVERSION = 1024;
const double VREF = 3.3;
const double ADC_RES = 1024;

// display set-up
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // This display does not have a reset pin accessible