#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
#define S0 19
#define S1 14
#define S2 4
#define S3 2
#define sensorOut 5
int frequency = 0;
int red, green, blue = 0;
int lastState = HIGH; // the previous state from the input pin
int currentState;     // the current reading from the input pin

void setup()
{
  pinMode(15, INPUT_PULLDOWN);
  pinMode(S0, OUTPUT);
pinMode(S1, OUTPUT);
pinMode(S2, OUTPUT);
pinMode(S3, OUTPUT);
pinMode(sensorOut, INPUT);
// Setting frequency-scaling to 20%
digitalWrite(S0,HIGH);
digitalWrite(S1,HIGH);
  Serial.begin(115200);
   lcd.begin(LCD_COLS, LCD_ROWS);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.write("DEVICE");
   lcd.setCursor(0, 1);
   lcd.write("POWERING ON");
   delay(2500);
   lcd.clear();
   updateLCD();
}

void loop()
{
  delay(200);
  updateLCD();
  // Setting red filtered photodiodes to be read
digitalWrite(S2,LOW);
digitalWrite(S3,LOW);
// Reading the output frequency
frequency = pulseIn(sensorOut, LOW);
// Printing the value on the serial monitor
Serial.print("R= ");//printing name
Serial.print(frequency);//printing RED color frequency
Serial.print(" ");
red = frequency;
delay(100);
// Setting Green filtered photodiodes to be read
digitalWrite(S2,HIGH);
digitalWrite(S3,HIGH);
// Reading the output frequency
frequency = pulseIn(sensorOut, LOW);
// Printing the value on the serial monitor
Serial.print("G= ");//printing name
Serial.print(frequency);//printing RED color frequency
Serial.print(" ");
green = frequency;
delay(100);
// Setting Blue filtered photodiodes to be read
digitalWrite(S2,LOW);
digitalWrite(S3,HIGH);
// Reading the output frequency
frequency = pulseIn(sensorOut, LOW);
// Printing the value on the serial monitor
Serial.print("B= ");//printing name
Serial.print(frequency);//printing RED color frequency
Serial.println(" ");
blue = frequency;
delay(100);
}

void updateLCD()
{
  currentState = digitalRead(15);
  if (lastState != currentState) {
    if(digitalRead(15) == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("RGB =" + red + " " + green + " " + blue);
      lcd.setCursor(0,1);
      lcd.print("");
      Serial.println(digitalRead(15));
      
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SCANNING MODE:");
    lcd.setCursor(0, 1);
    lcd.print("OFF");
    Serial.println(digitalRead(15));
  }
  }
  
  lastState = currentState;
}