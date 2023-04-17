#include <Arduino.h>
#include "constants.h"
#include <Adafruit_SSD1306.h>
#include "Wire.h"

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// timer interrupt example
HardwareTimer timer1(TIM1);
HardwareTimer timer2(TIM2);
bool ledOn = false;
volatile bool send_current = false;
volatile bool calc_current = false;
String current_str = "";
volatile double running_voltage[2];

void OnTimer1Interrupt() {
    ledOn = !ledOn;
    digitalWrite(PC13, ledOn ? HIGH : LOW);
    //calc_current = true;
}

void OnTimer2Interrupt() {
  if (calc_current == false) {
    calc_current = true;
  }
  // send_current = true;
}
// -------------------------------------------

// Variable to store the value of the analog in pin.
volatile uint16_t value = 0;
volatile uint32_t duty_cycle = 0;
volatile double analog_val = 0;
volatile bool fwd = false;

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

void init_timers() {
    // timer1 interrupt example
    pinMode(PC13, OUTPUT);
    // Configure timer
    // clk frequency = 72MHz
    // current frequency factor = 72MHz * 1ms (10 cycles) = 71999 -> but split this into prescale and overflow
    timer1.setPrescaleFactor(2564); // Set prescaler to 2564 => timer frequency = 72MHz/2564 = 28081 Hz (from prediv'd by 1 clocksource of 168 MHz)
    timer1.setOverflow(28); // Set overflow to 28 => timer frequency = 28081 Hz / 28 = 1kHz
    timer1.attachInterrupt(OnTimer1Interrupt);
    timer1.refresh(); // Make register changes take effect
    timer1.resume(); // Start

    // timer2 - this is the one that will serial print the next x values, slower loop every 10 ms
    timer2.setPrescaleFactor(2564); // 28081
    timer2.setOverflow(280); // 100Hz
    timer2.attachInterrupt(OnTimer2Interrupt);
    timer2.refresh(); // Make register changes take effect
    timer2.resume(); // Start
}

void setup() {
  // put your setup code here, to run once:
    Serial1.begin(250000);
    init_pins();
    set_up_display();
    init_timers();
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
  display_handler.println(timer1.getTimerClkFreq());
  
  display_handler.println(loop_counter++);
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
  running_voltage[0] += analog_val;
  running_voltage[1] += 1;
}

void loop() {
  drive_motor();
  read_vsense();
  display_data();
  if (calc_current) {
    calc_current = false;
    double v_avg = running_voltage[0] / running_voltage[1];
    double i_avg = v_avg/GAIN/R_SENSE;
    Serial1.println(i_avg-3.30);
    running_voltage[0] = 0;
    running_voltage[1] = 0;
  }
  if (send_current) {
    // calculate the average of the values
      Serial1.println(current_str);
      current_str = "";
      send_current = false;
  }
}