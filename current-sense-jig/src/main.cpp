#include <Arduino.h>
#include "constants.h"
#include <Adafruit_SSD1306.h>
#include "Wire.h"

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variable to store the value of the analog in pin.
volatile uint16_t value = 0;
volatile uint32_t duty_cycle = 0;
volatile double analog_val = 0;
volatile bool fwd = false;

// Example 5 - Receive with start- and end-markers combined with parsing

const byte numChars = 128;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

// variables to hold the parsed data
int length = 0;
int receivedLength = 0;
// char payloadFromPC[numChars] = {0};
// char tempPayload[numChars] = {0};
float checksum = 0;
float xpos = 0;
float ypos = 0;
float xvel = 0;
float yvel = 0;

bool newData = false;
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
    Serial1.begin(115200);
    
    init_pins();
    set_up_display();
}

void drive_motor() {
  duty_cycle = analogRead(EXT_DUTY_CYCLE);
  pwm_start(PWM, PWM_FREQ_HZ, duty_cycle, RESOLUTION_10B_COMPARE_FORMAT);
  // put your main code here, to run repeatedly:
  digitalWrite(DIR, LOW); // LOW = A->B, HIGH = B->A as indicated on Cytron
  // delay(2000);
  // digitalWrite(DIR, HIGH);
  // delay(2000);
}

void display_data() {
  //Display the read signal's raw value (12 bit: 0 to 4096)
  display_handler.clearDisplay();
  display_handler.setCursor(0, 0);
  display_handler.println(loop_counter++);
  display_handler.println(analog_val);
  display_handler.print("Forward: ");
  display_handler.println(fwd);
  display_handler.print(duty_cycle);
  display_handler.display();
  Serial1.println(analog_val);
}

void recvWithStartEndMarkers() {
    static bool recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial1.available() > 0 && newData == false) {
        rc = Serial1.read();

        if (recvInProgress == true) {
            if (rc == startMarker) {
                ndx = 0;
            }
            else if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                receivedLength = ndx;
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars, ",");      // get the first part - the string
    length = atoi(strtokIndx);
    // strcpy(length, strtokIndx); // copy it to messageFromPC
 
    // strtokIndx = strtok(NULL, "],"); // this continues where the previous call left off
    // strcpy(payloadFromPC, strtokIndx);

    strtokIndx = strtok(NULL, ",");
    xpos = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    ypos = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    xvel = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    yvel = atof(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    checksum = atof(strtokIndx);
}

bool checkLength() {
    return (length == receivedLength);
}

bool checkSum() {
    return (checksum == xpos + ypos + xvel + yvel);
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
  display_data();

  read_vsense();

  // recvWithStartEndMarkers();
  // if (newData == true) {
  //   strcpy(tempChars, receivedChars);
  //       // this temporary copy is necessary to protect the original data
  //       //   because strtok() used in parseData() replaces the commas with \0
  //   // strcpy(tempPayload, payloadFromPC);
  //   parseData();
  //   if (checkLength() && checkSum()) {
  //       // Serial1.println(1);
  //       Serial1.print("ACK");
  //       Serial1.println(loop_counter);
  //   } else {
  //       // Serial1.println(24);
  //       Serial1.println(receivedChars);
  //   }
  //   // showParsedData();
  //   newData = false;
  // }
}