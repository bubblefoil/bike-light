#include <FastLED.h>

// Total number of LEDs in the strip
#define NUM_LEDS 144
// Index of the first LED of the tail light
#define REAR_LEDS_START 72
// Controls the LED strip
#define DATA_PIN 6

#define RX 11
#define TX 10
// Digital input pins for buttons
#define pinIN1 2
#define pinIN2 4

// Light modes
// Number of modes. For modulo division when cycling through the modes.
const byte N_MODES = 4;
const byte OFF = 0;
const byte DAY = 1;
const byte NIGHT = 2;
const byte PARK = 3;

byte lightMode = OFF;
byte lastLightMode = OFF;

// Number of head/tail light LEDs. These shine at full brightness.
const byte N_FRONTAL = 6;
// Number of turn signal LEDs
const byte N_TURN_SIG = 12;

// Ranges of the head/tail lights
// Find the symetrical range of N_FRONTAL LEDs within the front part of the strip. N_FRONTAL to each side from the center.
// Calculated in the full front range and divided by bitshift in the end.
const byte frontLightA = (REAR_LEDS_START - (N_FRONTAL << 1)) >> 1;
const byte frontLightB = (REAR_LEDS_START + (N_FRONTAL << 1)) >> 1;
// The same calculation as above, only shifted to the start of the strip and back to the rear part in the end.
const byte rearLightA = (((NUM_LEDS - REAR_LEDS_START) - (N_FRONTAL << 1)) >> 1) + REAR_LEDS_START;
const byte rearLightB = (((NUM_LEDS - REAR_LEDS_START) + (N_FRONTAL << 1)) >> 1) + REAR_LEDS_START;

// Memory for all LED values
CRGB leds[NUM_LEDS];

// current state of the button
int buttonState1 = 0;
// previous state of the button
int lastButtonState1 = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino ON.");

  // Setup button pins
  pinMode(pinIN1, INPUT);
  pinMode(pinIN2, INPUT);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  updateLights();
}

void loop()
{
  // Detect a button press by checking for its state change
  buttonState1 = digitalRead(pinIN1);
  if (buttonState1 != lastButtonState1)
  {
    if (buttonState1 == HIGH)
    {
      lightMode = (lastLightMode + 1) % N_MODES;
    }
    delay(50);
  }
  lastButtonState1 = buttonState1;

  // Update lights if the mode was changed, otherwise do nothing
  if (lightMode != lastLightMode)
  {
    Serial.print("Light mode: ");
    Serial.println(lightMode);
    updateLights();
    lastLightMode = lightMode;
    // Add some stability
    delay(200);
  }
}

void updateLights()
{
  switch (lightMode)
  {
  case DAY:
    day();
    break;
  case NIGHT:
    night();
    break;
  case PARK:
    park();
    break;
  case OFF:
  default:
    off();
    break;
  }
  FastLED.show();
}

void off()
{
  FastLED.clear(false);
}

/**
 * Activates night light mode. Dim light on sides, full brightness on nose and tail.
*/
void park()
{
  off();
  for (int led = 0; led < REAR_LEDS_START; led++)
  {
    leds[led] = CHSV(32, 20, 10);
  }
  for (int led = REAR_LEDS_START; led < NUM_LEDS; led++)
  {
    leds[led] = CHSV(0, 255, 15);
  }
}

/**
 * Activates day light mode.
*/
void day()
{
  off(); //Reset the sides
  frontalArea();
}

/**
 * Activates night light mode. Dim light on sides, full brightness on nose and tail.
*/
void night()
{
  for (int led = 0; led < REAR_LEDS_START; led++)
  {
    leds[led] = 0x0a0a0a;
  }
  for (int led = REAR_LEDS_START; led < NUM_LEDS; led++)
  {
    leds[led] = 0x330000;
  }
  frontalArea();
}

/**
 * Lights up the frontal area of both head and tail light with white and red, respectively.
*/
void frontalArea()
{
  for (int led = frontLightA; led < frontLightB; led++)
  {
    leds[led] = CRGB::White;
  }
  for (int led = rearLightA; led < rearLightB; led++)
  {
    leds[led] = CRGB::Red;
  }
}
