#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Define the number of sliders and LED strip settings
const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4};
const int ledPin = 8;  // Pin where the WS2812 LEDs are connected
const int numLEDs = NUM_SLIDERS;  // One LED per slider
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLEDs, ledPin, NEO_GRB + NEO_KHZ800);

const int numReadings = 20;
int sliderValues[NUM_SLIDERS];
int sliderReadings[NUM_SLIDERS][numReadings];
int readIndex[NUM_SLIDERS] = {0};
int total[NUM_SLIDERS] = {0};
int averageSliderValues[NUM_SLIDERS] = {0};
int analogSliderValues[NUM_SLIDERS];

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    for (int j = 0; j < numReadings; j++) {
      sliderReadings[i][j] = 0;
    }
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    sliderValues[i] = analogRead(analogInputs[i]);
    updateAverage(i, sliderValues[i]);
  }
  


  updateLEDs();
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug
  delay(10);
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


void updateAverage(int sliderIndex, int newValue) {
  total[sliderIndex] = total[sliderIndex] - sliderReadings[sliderIndex][readIndex[sliderIndex]];
  sliderReadings[sliderIndex][readIndex[sliderIndex]] = newValue;
  total[sliderIndex] = total[sliderIndex] + sliderReadings[sliderIndex][readIndex[sliderIndex]];
  readIndex[sliderIndex] = (readIndex[sliderIndex] + 1) % numReadings;
  averageSliderValues[sliderIndex] = total[sliderIndex] / numReadings;
}

void updateLEDs() {
  if (averageSliderValues[0] < 10) {
    strip.clear();
    strip.show();
    return;
  }

  for (int i = 0; i < NUM_SLIDERS; i++) {
    if (averageSliderValues[i] < 10) {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Turn off the LED
    } else {
      int brightness = map(averageSliderValues[i], 10, 1023, 0, 255); // Adjust mapping for values above 10
      strip.setPixelColor(i, strip.Color(255 - brightness, 0, brightness)); // Red to Blue gradient
    }
  }
  strip.show();
}
