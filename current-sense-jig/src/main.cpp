#include <Arduino.h>
#include "constants.h"
#include <Adafruit_SSD1306.h>
#include "Wire.h"

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// -------------------------------------------
// timer interrupt example
HardwareTimer timer(TIM1);
bool ledOn = false;

void OnTimer1Interrupt() {
    ledOn = !ledOn;
    digitalWrite(PC13, ledOn ? HIGH : LOW);
}
// -------------------------------------------

// Variable to store the value of the analog in pin.
volatile uint16_t value = 0;
volatile uint32_t duty_cycle = 0;
volatile double analog_val = 0;
volatile bool fwd = false;

// Variables for encoders
volatile int master_count = 0; // universal count

// Variable to show the program is running
uint32_t loop_counter = 0;

void encoderIRQ() {
  loop_counter = 0;
  // add 1 to count for CW
  if (digitalRead(ENC_1) && !digitalRead(ENC_2)) {
    master_count++ ;
  }
  // subtract 1 from count for CCW
  else if (digitalRead(ENC_1) && digitalRead(ENC_2)) {
    master_count-- ;
  } 
}

void init_pins() {
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(EXT_DUTY_CYCLE, INPUT);
  pinMode(V_SENSE, INPUT);
  pinMode(CSA_GND, INPUT);
  pinMode(ENC_1, INPUT);
  pinMode(ENC_2, INPUT);
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
    Serial1.begin(250000);
    
    init_pins();
    set_up_display();

    attachInterrupt(digitalPinToInterrupt(ENC_1), encoderIRQ, RISING);

    // timer interrupt example
    // pinMode(PC13, OUTPUT);
    // Configure timer
    // timer.setPrescaleFactor(2564); // Set prescaler to 2564 => timer frequency = 168MHz/2564 = 65522 Hz (from prediv'd by 1 clocksource of 168 MHz)
    // timer.setOverflow(32761); // Set overflow to 32761 => timer frequency = 65522 Hz / 32761 = 2 Hz
    // timer.attachInterrupt(OnTimer1Interrupt);
    // timer.refresh(); // Make register changes take effect
    // timer.resume(); // Start
}

void drive_motor() {
  duty_cycle = analogRead(EXT_DUTY_CYCLE);
  pwm_start(PWM, PWM_FREQ_HZ, duty_cycle, RESOLUTION_10B_COMPARE_FORMAT);
  // put your main code here, to run repeatedly:
  digitalWrite(DIR, LOW); // LOW = A->B, HIGH = B->A as indicated on Cytron
}

void display_data() {
  //Display the read signal's raw value (12 bit: 0 to 4096)
  display_handler.clearDisplay();
  display_handler.setCursor(0, 0);
  display_handler.println(loop_counter);

  // Display encoder value
  display_handler.print("Encoder count: ");
  display_handler.println(master_count);

  display_handler.println(analog_val);

  display_handler.print("Forward: ");
  display_handler.println(fwd);

  display_handler.print(duty_cycle);

  display_handler.display();
}

void read_vsense() {
  //Read the PWM signal as analog in
  value = analogRead(V_SENSE) - analogRead(CSA_GND); // 0 to 1024
  analog_val = value * VREF / ADC_RES; // ADC is 10 bit although it's supposed to be 12?
  fwd = true;
  if (analog_val < VREF/2) {
    fwd = false;
  }
}

void loop() {
  drive_motor();
  read_vsense();
  display_data();
  ++loop_counter;
  // Serial1.println(analog_val);
}
