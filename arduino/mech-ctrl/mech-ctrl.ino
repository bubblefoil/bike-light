#include <FastLED.h>
#include "Button.h"

// Total number of LEDs in the strip
#define NUM_LEDS 36
// Control the LED strips
#define DATA_PIN_FL 3
#define DATA_PIN_FR 4
#define DATA_PIN_BL 5
#define DATA_PIN_BR 6

// TBD - Bluetooth communication
#define RX 11
#define TX 10

// Digital input pins for buttons
#define pinLightMode 8
#define pinTurnLeft 9
#define pinTurnRight 7

const int pinPhotoResistor = A0;

// Light modes
// Number of light modes. For modulo division when cycling through the modes.
const byte N_MODES = 4;
const byte OFF = 0;
const byte DAY = 1;
const byte NIGHT = 2;
const byte PARK = 3;

// TODO Make this as an abstract class with updateLights method and brightness attribute.
byte lightMode = OFF;
byte lastLightMode = OFF;

//Left blinker
const byte LEFT = 11;
//Right blinker
const byte RIGHT = 12;
//Blinker light hue in the range from 0 - red to 255 - red again
const byte BLINKER_HUE = 24;
// Total duration of a single ON/OFF blink in milliseconds.
const unsigned long turnSignalInterval = 1000L;

// Number of head/tail light LEDs. These shine at full brightness.
const byte N_FRONTAL = 6;
// Number of turn signal LEDs
const byte N_TURN_SIG = 12;

// Memory for all LED values
CRGB ledsFL[NUM_LEDS];
CRGB ledsFR[NUM_LEDS];
CRGB ledsBL[NUM_LEDS];
CRGB ledsBR[NUM_LEDS];

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
  activateModeChange(buttonLightMode);
}

void activateModeChange(Button &b)
{
  b.releaseHandler(nextMode);
}

void activateBrightnessChangeMode(Button &b)
{
  // Deactivate click handler to ignore the last click, triggered by button hold.
  // This handler will reactivate the original light mode change click handler.
  b.releaseHandler(activateModeChange);
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
  buttonLightMode.releaseHandler(nextMode);
  buttonLightMode.holdHandler(activateBrightnessChangeMode, 300);
  //TODO try reactivating mode change on release. Modify the lib to keep firing hold events. Use this handler to change brightness.

  //Make sure lights are reset when a blinker is turned off.
  buttonBlinkerLeft.releaseHandler(turnOffBlinker);
  buttonBlinkerRight.releaseHandler(turnOffBlinker);

  // Temp ambient light sensor test
  buttonBlinkerLeft.pressHandler(readAmbientLight);
  buttonBlinkerRight.pressHandler(readAmbientLight);

  FastLED.addLeds<NEOPIXEL, DATA_PIN_FL>(ledsFL, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_FR>(ledsFR, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_BL>(ledsBL, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_BR>(ledsBR, NUM_LEDS);

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
  FastLED.show();
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
  fill_solid(ledsFL, NUM_LEDS, col);
}

void frontRight(const CHSV &col)
{ 
  fill_solid(ledsFR, NUM_LEDS, col);
}

void rearLeft(const CHSV &col)
{
  fill_solid(ledsBL, NUM_LEDS, col);
}

void rearRight(const CHSV &col)
{
  fill_solid(ledsBR, NUM_LEDS, col);
}

/**
 * Activates parking lights mode. Dim red/white lights all around.
*/
void park()
{
  off();
  frontLeft(CHSV(32, 20, globalBrightness >> 1));
  frontRight(CHSV(32, 20, globalBrightness >> 1));
  rearLeft(CHSV(0, 255, globalBrightness >> 1));
  rearRight(CHSV(0, 255, globalBrightness >> 1));
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
  frontLeft(CHSV(0, 0, sideBrightness));
  frontRight(CHSV(0, 0, sideBrightness));
  rearLeft(CHSV(0, 255, sideBrightness));
  rearRight(CHSV(0, 255, sideBrightness));
  frontalArea();
}

/**
 * Lights up the frontal area of both head and tail light with white and red, respectively.
*/
void frontalArea()
{
  fill_solid(&ledsFL[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::White);
  fill_solid(&ledsFR[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::White);
  fill_solid(&ledsBL[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::Red);
  fill_solid(&ledsBR[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::Red);
}
