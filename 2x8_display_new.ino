#include <LiquidCrystal.h>
#include "MAX30105.h"
#include "heartRate.h"
#define WARN_RATE 120

MAX30105 particleSensor;

// Initialize the library with the pins connected to the LCD
// RS = 7, E = 8, D4 = 9, D5 = 10, D6 = 11, D7 = 12
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


const byte RATE_SIZE = 8; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute = 0;
int beatAvg = 0;

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  // Set up the LCD's number of columns and rows
  lcd.begin(8, 2);
  
  // Print a message to the LCD
  lcd.print("BPM:");
  lcd.setCursor(0, 1); // Move to the second line
  lcd.print("    ");

  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

}

void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    } else{
      beatsPerMinute = 0;
    }

  if(beatAvg >= WARN_RATE){
    digitalWrite(2, HIGH);
  }
  if(beatAvg < WARN_RATE){
    digitalWrite(2, LOW);
      }
  }
  lcd.setCursor(0, 1);
  lcd.print("   ");
  lcd.setCursor(0, 1);
  lcd.print(beatAvg);
}