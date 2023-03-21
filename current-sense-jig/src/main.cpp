#include <Arduino.h>
#include "constants.h"
#include <Adafruit_SSD1306.h>
#include "Wire.h"

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variable to store the value of the analog in pin.
volatile uint16_t value = 0;
volatile uint32_t duty_cycle = 0;

// HardwareSerial Serial1(PA10, PA9);

// Variable to show the program is running
uint32_t loop_counter = 0;


void init_pins() {
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);
  digitalWrite(PWM, LOW);
  digitalWrite(DIR, LOW);
}

void set_up_display()
{
  // setting up the display
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display_handler.clearDisplay();
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  display_handler.setCursor(0, 0);
  display_handler.display();
}

void setup() {
  // put your setup code here, to run once:
    // Serial1.begin(128000);
    init_pins();
    set_up_display();
}

void loop() {
  duty_cycle = analogRead(EXT_DUTY_CYCLE);
  pwm_start(PWM, PWM_FREQ_HZ, duty_cycle, RESOLUTION_10B_COMPARE_FORMAT);
  // put your main code here, to run repeatedly:
  digitalWrite(DIR, LOW); // LOW = A->B, HIGH = B->A as indicated on Cytron
  // delay(2000);
  // digitalWrite(DIR, HIGH);
  // delay(2000);

  //Read the PWM signal as analog in
  value = analogRead(V_SENSE) - analogRead(CSA_GND); // 0 to 1024
  double analog_val = value * VREF / ADC_RES; // ADC is 10 bit although it's supposed to be 12?
  bool fwd = true;
  if (analog_val < VREF/2) {
    fwd = false;
  }

  //Display the read signal's raw value (12 bit: 0 to 4096)
  display_handler.clearDisplay();
  display_handler.setCursor(0, 0);
  display_handler.println(loop_counter++);
  display_handler.println(analog_val);
  display_handler.print("Forward: ");
  display_handler.println(fwd);
  display_handler.print(duty_cycle);
  display_handler.display();
}