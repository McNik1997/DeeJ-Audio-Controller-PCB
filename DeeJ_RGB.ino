#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Define the number of sliders and LED strip settings
const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4};
const int ledPin = 8;  // Pin where the WS2812 LEDs are connected
const int numLEDs = NUM_SLIDERS;  // One LED per slider
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLEDs, ledPin, NEO_GRB + NEO_KHZ800);

int analogSliderValues[NUM_SLIDERS];

void setup() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug
  updateLEDs();

  delay(10);
}

void updateLEDs() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    int brightness = map(analogSliderValues[i], 0, 1023, 0, 255);
    strip.setPixelColor(i, strip.Color(255 - brightness, 0, brightness)); // Corrected: Red to Blue gradient
  }
  strip.show();
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}