#include <FastLED.h>
#include "Button.h"

// Total number of LEDs in the strip
#define NUM_LEDS 144
// Index of the first LED of the tail light
#define REAR_LEDS_START 72
#define LEDS_FL 0
#define LEDS_FR 36
#define LEDS_BL 72
#define LEDS_BR 108
// Controls the LED strip
#define DATA_PIN 6

//Blinker light hue in the range from 0 - red to 255 - red again
#define BLINKER_HUE 24

#define RX 11
#define TX 10
// Digital input pins for buttons
#define pinLightMode 2
#define pinTurnLeft 4
#define pinTurnRight 7

const int pinPhotoResistor = A0;

// Light modes
// Number of light modes. For modulo division when cycling through the modes.
const byte N_MODES = 4;
const byte OFF = 0;
const byte DAY = 1;
const byte NIGHT = 2;
const byte PARK = 3;

byte lightMode = OFF;
byte lastLightMode = OFF;

//Left blinker
const byte LEFT = 11;
//Right blinker
const byte RIGHT = 12;

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

// Total duration of a single ON/OFF blink in milliseconds.
const unsigned long turnSignalInterval = 1000L;

// Forward prototype declaration because of the default parameter.
void updateLights(bool show = true);

//Temporary variable to test brightness control.
byte globalBrightness = 255;

// Controls light mode. Holding the button changes brightness.
Button buttonLightMode = Button(pinLightMode, BUTTON_PULLDOWN, true, 50);
Button buttonBlinkerLeft = Button(pinTurnLeft, BUTTON_PULLDOWN, true, 50);
Button buttonBlinkerRight = Button(pinTurnRight, BUTTON_PULLDOWN, true, 50);

void nextMode(Button &b)
{
  lightMode++;
  lightMode %= N_MODES;
  Serial.print("Light mode: ");
  Serial.println(lightMode);
  updateLights();
  activateModeChange();
}

void activateModeChange()
{
  buttonLightMode.clickHandler(nextMode);
}

void activateBrightnessChangeMode(Button &b)
{
  // Deactivate click handler to ignore the last click, triggered by button hold.
  // This handler will reactivate the original light mode change click handler.
  b.clickHandler(activateModeChange);
}

void turnOffBlinker(Button &b)
{
  updateLights();
}

void readAmbientLight(Button &b)
{
  Serial.print("Light:");
  Serial.println(analogRead(pinPhotoResistor));
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino ON.");

  pinMode(pinPhotoResistor, INPUT); // Set pinPhotoResistor - A0 pin as an input (optional)

  // Setup buttons
  buttonLightMode.clickHandler(nextMode);
  buttonLightMode.holdHandler(activateBrightnessChangeMode, 300);

  //Make sure lights are reset when a blinker is turned off.
  buttonBlinkerLeft.releaseHandler(turnOffBlinker);
  buttonBlinkerRight.releaseHandler(turnOffBlinker);

  // Temp solution
  buttonBlinkerLeft.pressHandler(readAmbientLight);
  buttonBlinkerRight.pressHandler(readAmbientLight);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  updateLights();
}

void loop()
{
  buttonLightMode.process();

  //todo Cannot use heldFor() because it checks whether hold event has already been fired. Use better timing here.
  if (buttonLightMode.heldFor(300) && buttonLightMode.holdTime() % 10 == 0)
  {
    globalBrightness++;
    Serial.println(globalBrightness);
    updateLights();
  }

  handleBlinkers();
}

void handleBlinkers()
{
  if (buttonBlinkerLeft.isPressed())
  {
    blink(buttonBlinkerLeft.holdTime(), LEFT);
  }
  else if (buttonBlinkerRight.isPressed())
  {
    blink(buttonBlinkerRight.holdTime(), RIGHT);
  }
}

/**
 * Returns true if we are in the first half of the blinker interval,
 * measured since the blinker was switched on.
 */
bool getBlinkerLightState(unsigned int switchOnTime)
{
  // Less than half the blinker interval elapsed?
  return (switchOnTime % turnSignalInterval < (turnSignalInterval / 2));
}

void blink(unsigned int blinkerOnTime, byte side)
{
  //Is the blinker light on or off at the moment?
  bool blinkerState = getBlinkerLightState(blinkerOnTime);

  if (!blinkerState)
  {
    updateLights(false);
  }

  byte brightness = blinkerState ? 255 : 0;
  CHSV blinkerColor = CHSV(BLINKER_HUE, 255, brightness);
  if (side == RIGHT)
  {
    frontRight(blinkerColor);
    rearRight(blinkerColor);
  }
  else if (side == LEFT)
  {
    frontLeft(blinkerColor);
    rearLeft(blinkerColor);
  }
  FastLED.show();
}

void updateLights(bool show)
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
  if (show)
  {
    FastLED.show();
  }
}

void off()
{
  FastLED.clear(false);
}

void frontLeft(const CHSV &col)
{
  fill_solid(leds, LEDS_FR, col);
}

void frontRight(const CHSV &col)
{
  fill_solid(&leds[LEDS_FR], REAR_LEDS_START - LEDS_FR, col);
}

void rearLeft(const CHSV &col)
{
  fill_solid(&leds[LEDS_BL], LEDS_BR - LEDS_BL, col);
}

void rearRight(const CHSV &col)
{
  fill_solid(&leds[LEDS_BR], NUM_LEDS - LEDS_BR, col);
}

/**
 * Activates parking lights mode. Dim red/white lights all around.
*/
void park()
{
  off();
  fill_solid(leds, REAR_LEDS_START, CHSV(32, 20, globalBrightness >> 1));
  fill_solid(&leds[REAR_LEDS_START], NUM_LEDS - REAR_LEDS_START, CHSV(0, 255, globalBrightness >> 1));
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
  //TODO Experimental ambient light controlled brightness. Does not update when ambient light changes. 
  byte sideBrightness = analogRead(pinPhotoResistor) >> 3; // 1/8 * 1024
  fill_solid(leds, REAR_LEDS_START, CHSV(0, 0, sideBrightness));
  fill_solid(&leds[REAR_LEDS_START], NUM_LEDS - REAR_LEDS_START, CHSV(0, 255, sideBrightness));
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
