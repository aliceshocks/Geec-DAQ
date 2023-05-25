/// PRE RACE  \\\
// remeasure wheel
//check reset button


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//conversion to fix the RPM being wrong
//in the form y=mx+b     fixed=m*raw + b

const float m = 0.8983353362875; //calibration
const float b = 10.1202308420759;

const float wheelDiam = 0.5; //in meters 
float circ = wheelDiam * 3.1415;

const int hall_pin = 2; // digital pin 2 is the hall pin
const int hall_thresh = 3; // set number of hall trips for RPM reading (higher improves accuracy)
int hall_count = 0;
unsigned long start_time = micros();
unsigned long end_time;
unsigned long time_passed;
float RPM;
float fixedRPM;
float velocity = 0;
unsigned long run_time = millis();
unsigned long mins =0.0;
unsigned long secs = 0.0;

int currentreading;
int pastreading;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  pinMode(hall_pin, INPUT_PULLUP);
  lcd.begin();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("THE GEEC!");
  lcd.setCursor(0, 1);
  delay(5000); // for capacitors
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("THE GEEC!");
  lcd.setCursor(0, 1);
  lcd.print("Ready!");
}

void loop() {
  currentreading = digitalRead(hall_pin);
  for (int i = 0; i<hall_thresh; i++){
    if(currentreading != pastreading){
    hall_count = hall_count + 1;
    }
    else if(hall_count >= hall_thresh){
      end_time = micros();   
      time_passed = (end_time - start_time)/1000000;
      RPM = (hall_count/time_passed)*60;
      //fixedRPM = (m*RPM) + b;
      velocity = (RPM * circ * 60)/1000;
      run_time = millis();
      secs = run_time/1000;
      mins = secs/60;
      secs %= 60;
      mins %= 60;
    
      lcd.clear();
      lcd.setCursor(0, 0);
      
      lcd.print(velocity);
      lcd.print("km/h");
      
      lcd.setCursor(0, 1);
      if (mins < 10) {
        lcd.print('0');
      }
      lcd.print(mins);
      lcd.print(':');
      if (secs < 10) {
        lcd.print('0');
      }
      lcd.print(secs);  
    
      start_time = micros();  
      hall_count = 0;
    }
  }
  pastreading = currentreading;
  delay(10);
}
