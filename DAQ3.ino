void(* resetFunc) (void) = 0;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//comment out this line to run in race mode
#define DEBUG 0

// CONSTANTS

//conversion to fix the RPM being wrong
//in the form y=mx+b     fixed=m*raw + b

const float m = 0.8983353362875;
const float b = 10.1202308420759;


#define numMessages 7

const char messages[numMessages][17] = {
  "Good Luck Aoife",
  "Do it 4 Ireland",
  "Nite @ Museum 2",
  "Aoife ur an icon",
  "You can do it",
  "Slay",
  "Go piss girl"
};
const float wheelDiam = 0.5; //in meters

float circ = wheelDiam * 3.1415;

// digital pin 2 is the hall pin
const int hall_pin = 2;

//number of magnets on the wheel
const int num_mags = 1;

//number of rotations to count and time, higher values will give smoother results, but less frequent updates
const int rotation_threshold = 3;


// set number of hall trips for RPM reading (higher improves accuracy)
int hall_thresh = rotation_threshold * num_mags;


float RPM = 0;
float velocity = 0;


// timeout detection
unsigned long zeroSpeedTimeout = 5 * 1000000; // 1 second in microseconds
unsigned long failureTimeout = zeroSpeedTimeout * 3; // time to wait before resetting


//LCD update timer  IN MILLISECONDS
unsigned long lastLCDUpdate = 0;

const unsigned long lcdUpdateWait = 250; // Idk what to set this


LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif
  randomSeed(analogRead(0));
  pinMode(hall_pin, INPUT_PULLUP);
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("THE GEEC!");
  lcd.setCursor(0, 1);
  lcd.print(messages[random(numMessages)]);
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("THE GEEC!");
  lcd.setCursor(0, 1);
  lcd.print("Ready!");

#ifdef DEBUG
  Serial.println(zeroSpeedTimeout);
  Serial.println(failureTimeout);
#endif
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
#ifdef DEBUG
      Serial.println("MAGNET");
#endif
      if (on_state == false) {
        on_state = true;
        hall_count += 1.0;
      }
    } else {
      on_state = false;
    }
    //check for timeout
#ifdef DEBUG
    Serial.print("start:");
    Serial.print(start);
    Serial.print("  micros: ");
    unsigned long mic = micros();
    Serial.print(mic);
    Serial.print("  diff: ");
    Serial.println(mic - start);

#endif

    if (micros() - start > zeroSpeedTimeout) {
      //the speed is zero
      velocity = 0.0;
#ifdef DEBUG
      Serial.println("IT'S ZERO!");
#endif
      if (micros() - start > failureTimeout) {
        resetFunc();
#ifdef DEBUG
        Serial.println("RESET");
#endif
      }
    }
    if (millis() - lastLCDUpdate > lcdUpdateWait) {
      updateLCD();
      lastLCDUpdate = millis();
    }
    delay(1);
  }
  unsigned long end_time = micros();
  unsigned long time_passed_us = end_time - start;
  float time_passed = (time_passed_us / 1000000.0);
  RPM = (hall_count / time_passed) * 60.0 / num_mags;
  RPM = fixRPM(RPM);
  velocity = RPM * circ * 60.0 / 1000.0;

  updateLCD();
  delay(1);
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(velocity);
  lcd.print("km/h");

  lcd.setCursor(0, 1);

  unsigned long seconds = millis() / 1000; //unsigned long
  unsigned long minutes = seconds / 60;         //unsigned long
  seconds %= 60;
  minutes %= 60;
  if (minutes < 10) {
    lcd.print('0');
  }
  lcd.print(minutes);
  lcd.print(':');

  if (seconds < 10) {
    lcd.print('0');
  }
  lcd.print(seconds);
#ifdef DEBUG
  Serial.println("LCD");
#endif
}

float fixRPM(float raw_RPM) {
  float fixed = m * raw_RPM + b;
  return fixed;
}
