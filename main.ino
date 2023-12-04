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

int redMin = 55; // Red minimum value
int redMax = 720; // Red maximum value
int greenMin = 65; // Green minimum value
int greenMax = 880 ; // Green maximum value
int blueMin = 55; // Blue minimum value
int blueMax = 680; // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;
int lastState = HIGH; // the previous state from the input pin
int currentState;

// Variables for final Color values
int redValue;
int greenValue;
int blueValue;

void setup() {
	// Set S0 - S3 as outputs
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
   pinMode(15, INPUT_PULLDOWN);

	// Set Sensor output as input
	pinMode(sensorOut, INPUT);

	// Set Frequency scaling to 20%
	digitalWrite(S0,HIGH);
	digitalWrite(S1,LOW);

	// Setup Serial Monitor
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

void loop() {
	// Read Red value
	redPW = getRedPW();
	// Map to value from 0-255
	redValue = map(redPW, redMin,redMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Read Green value
	greenPW = getGreenPW();
	// Map to value from 0-255
	greenValue = map(greenPW, greenMin,greenMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Read Blue value
	bluePW = getBluePW();
	// Map to value from 0-255
	blueValue = map(bluePW, blueMin,blueMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Print output to Serial Monitor
	Serial.print("Red = ");
	Serial.print(redValue);
	Serial.print(" - Green = ");
	Serial.print(greenValue);
	Serial.print(" - Blue = ");
	Serial.println(blueValue);
  updateLCD();
}


// Function to read Red Pulse Widths
int getRedPW() {
	// Set sensor to read Red only
	digitalWrite(S2,LOW);
	digitalWrite(S3,LOW);
	// Define integer to represent Pulse Width
	int PW;
	// Read the output Pulse Width
	PW = pulseIn(sensorOut, LOW);
	// Return the value
	return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
	// Set sensor to read Green only
	digitalWrite(S2,HIGH);
	digitalWrite(S3,HIGH);
	// Define integer to represent Pulse Width
	int PW;
	// Read the output Pulse Width
	PW = pulseIn(sensorOut, LOW);
	// Return the value
	return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
	// Set sensor to read Blue only
	digitalWrite(S2,LOW);
	digitalWrite(S3,HIGH);
	// Define integer to represent Pulse Width
	int PW;
	// Read the output Pulse Width
	PW = pulseIn(sensorOut, LOW);
	// Return the value
	return PW;
}

void updateLCD()
{
  currentState = digitalRead(15);
  if (lastState != currentState) {
    if(digitalRead(15) == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("RED");
      lcd.setCursor(4, 0);
      lcd.print("GREEN");
      lcd.setCursor(10, 0);
      lcd.print("BLUE");
      lcd.setCursor(0,1);
      lcd.print(redValue);
      lcd.setCursor(4, 1);
      lcd.print(greenValue);
      lcd.setCursor(10, 1);
      lcd.print(blueValue);
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