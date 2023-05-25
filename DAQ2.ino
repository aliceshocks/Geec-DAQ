#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//comment out this line to run in race mode
//#define DEBUG 1

// CONSTANTS

//conversion to fix the RPM being wrong
//in the form y=mx+b     fixed=m*raw + b

const float m = 0.8983353362875;
const float b = 10.1202308420759;



const char messages[4][16] = {
  "Good Luck Aoife",
  "You can do it!",
  "Be Efficient!",
  "Nite @ Museum 2"
};
const float wheelDiam = 0.52; //in meters

float circ = wheelDiam * 3.1415;

// digital pin 2 is the hall pin
const int hall_pin = 2;
// set number of hall trips for RPM reading (higher improves accuracy)
const int hall_thresh = 3;


float RPM = 0;
float velocity = 0;



LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  randomSeed(analogRead(0));
  pinMode(hall_pin, INPUT_PULLUP);
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("THE GEEC!");
  lcd.setCursor(0, 1);
  lcd.print(messages[random(4)]);
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("THE GEEC!");
  lcd.setCursor(0, 1);
  lcd.print("Ready!");
}


void loop()
{
  // preallocate values for tach
  float hall_count = 0.0;
  unsigned long start = micros();
  bool on_state = false;
  // counting number of times the hall sensor is tripped
  // but without double counting during the same trip
  while (hall_count < hall_thresh) {
    if (digitalRead(hall_pin) == 0) {
      if (on_state == false) {
        on_state = true;
        hall_count += 1.0;
      }
    } else {
      on_state = false;
    }
    delay(1);
  }
  unsigned long end_time = micros();
  unsigned long time_passed_us = end_time - start;
  float time_passed = (time_passed_us / 1000000.0);
  RPM = (hall_count / time_passed) * 60.0;
  RPM = fixRPM(RPM);
  velocity = RPM * circ * 60.0 / 1000.0;
  lcd.clear();
  lcd.setCursor(0, 0);

  #ifdef DEBUG
  lcd.print("RPM: ");
  lcd.print(RPM);
  lcd.setCursor(0, 1);
  #endif
  lcd.print(velocity);
  lcd.print("km/h");

  delay(1);
}

float fixRPM(float raw_RPM){
  float fixed = m * raw_RPM + b;
  return fixed;
}
