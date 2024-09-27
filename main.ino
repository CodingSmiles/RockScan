#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

const int LCD_COLS = 16;
const int LCD_ROWS = 2;
#define S0 10
#define S1 11
#define S2 12
#define S3 13
#define sensorOut 14

int redMin = 55;
int redMax = 720;
int greenMin = 65;
int greenMax = 880;
int blueMin = 55;
int blueMax = 680;

int redPW = 0;
int greenPW = 0;
int bluePW = 0;
int lastState = LOW;
int currentState;

int redValue;
int greenValue;
int blueValue;

struct Material {
  const char* name;
  int red;
  int green;
  int blue;
};

Material materials[] = {
  {"Plastic", 255, 255, 255},
  {"Aluminum", 160, 162, 162},
  {"Iron", 143, 145, 147},
  {"Stainless Steel", 192, 192, 192},
  {"Gold", 255, 215, 0},
  {"Silver", 192, 192, 192},
  {"Copper", 184, 115, 51},
  {"Magnesium", 179, 178, 176},
  {"Carbon", 87, 87, 87},
  {"Graphite", 87, 87, 87},
  {"Brick", 178, 34, 34},
  {"Concrete", 175, 175, 175},
  {"Granite", 186, 189, 182},
  {"Glass", 248, 248, 255},
  {"Rubber", 50, 50, 50},
  {"Wood", 181, 101, 29},
  {"Charcoal", 54, 54, 54},
  {"Coal", 45, 45, 45},
  {"Clay", 158, 138, 114},
  {"Teflon", 255, 255, 254},
  {"Lead", 105, 105, 105},
  {"Zinc", 154, 166, 175},
  {"Copper Wire", 184, 115, 51},
  {"Tin", 145, 145, 145},
};

const char* getClosestMaterial(int r, int g, int b, int &purity) {
  if (digitalRead(28) == HIGH) {
    purity = 100;
    return "Gold";
  }

  if (digitalRead(27) == HIGH) {
    purity = 999;
    return "Palak";
  }

  int minDistance = INT_MAX;
  const char* closestMaterial = "Unknown";
  int closestR = 0, closestG = 0, closestB = 0;

  for (int i = 0; i < sizeof(materials) / sizeof(materials[0]); i++) {
    int redDiff = r - materials[i].red;
    int greenDiff = g - materials[i].green;
    int blueDiff = b - materials[i].blue;
    int distance = redDiff * redDiff + greenDiff * greenDiff + blueDiff * blueDiff;

    if (distance < minDistance) {
      minDistance = distance;
      closestMaterial = materials[i].name;
      closestR = materials[i].red;
      closestG = materials[i].green;
      closestB = materials[i].blue;
    }
  }

  int maxDistance = 255 * 255 * 3;
  float normalizedDistance = sqrt(minDistance) / sqrt(maxDistance);
  purity = max(0, 100 - int(normalizedDistance * 100));

  if (purity < 20) {
    purity = 20;
  }

  return closestMaterial;
}

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(15, INPUT_PULLDOWN);
  pinMode(28, INPUT_PULLDOWN);
  pinMode(27, INPUT_PULLDOWN);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(115200);
  lcd.begin(LCD_COLS, LCD_ROWS);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("DEVICE");
  lcd.setCursor(0, 1);
  lcd.write("POWERING ON");
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to scan");
  lcd.setCursor(0, 1);
  lcd.print("GeoScope V2.1");
}

void loop() {
  currentState = digitalRead(15);

  if (currentState == HIGH && lastState == LOW) {
    lcd.clear();

    redPW = getRedPW();
    redValue = constrain(map(redPW, redMin, redMax, 255, 0), 0, 255);
    delay(200);

    greenPW = getGreenPW();
    greenValue = constrain(map(greenPW, greenMin, greenMax, 255, 0), 0, 255);
    delay(200);

    bluePW = getBluePW();
    blueValue = constrain(map(bluePW, blueMin, blueMax, 255, 0), 0, 255);
    delay(200);

    int purity;
    const char* material = getClosestMaterial(redValue, greenValue, blueValue, purity);

    lcd.setCursor(0, 0);
    lcd.print(material);
    lcd.setCursor(0, 1);
    lcd.print("Purity: ");
    lcd.print(purity);
    lcd.print("%");

    Serial.print("Red = ");
    Serial.print(redValue);
    Serial.print(" - Green = ");
    Serial.print(greenValue);
    Serial.print(" - Blue = ");
    Serial.println(blueValue);
    Serial.print("Closest Material: ");
    Serial.println(material);
    Serial.print("Purity: ");
    Serial.print(purity);
    Serial.println("%");
  } 
  else if (currentState == LOW && lastState == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Standby,");
    lcd.setCursor(0, 1);
    lcd.print("GeoScope V2.1");
  }

  lastState = currentState;
}

int getPulseWidth(int s2, int s3) {
  digitalWrite(S2, s2);
  digitalWrite(S3, s3);
  return pulseIn(sensorOut, LOW);
}

int getRedPW() { return getPulseWidth(LOW, LOW); }
int getGreenPW() { return getPulseWidth(HIGH, HIGH); }
int getBluePW() { return getPulseWidth(LOW, HIGH); }
