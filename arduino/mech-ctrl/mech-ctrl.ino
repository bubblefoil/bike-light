#include <FastLED.h>

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

// current state of the button
int buttonState1 = 0;
// previous state of the button
int lastButtonState1 = 0;

// Total duration of a single ON/OFF blink in milliseconds.
const unsigned long turnSignalInterval = 1000L;
// Elapsed time since switching a blinker on.
unsigned long blinkerSwitchOnTime = 0L;
bool lastBlinkerLightState = false;

// Forward prototype declaration because of the default parameter.
void updateLights(bool show = true);

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino ON.");

  // Setup button pins
  pinMode(pinLightMode, INPUT);
  pinMode(pinTurnLeft, INPUT);
  pinMode(pinTurnRight, INPUT);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  updateLights();
}

void loop()
{
  // Detect a button press by checking for its state change
  buttonState1 = digitalRead(pinLightMode);
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

  handleBlinkers();
}

void handleBlinkers()
{
  byte signal = 0;
  if (digitalRead(pinTurnLeft) == HIGH)
  {
    signal = LEFT;
    // blink(LEFT);
    frontLeft(CHSV(32, 255, 255));
    rearLeft(CHSV(32, 255, 255));
  }
  else if (digitalRead(pinTurnRight) == HIGH)
  {
    signal = RIGHT;
  }
  else
  {
    if (blinkerSwitchOnTime != 0)
    {
      // Reset to the state before blinker
      updateLights();
    }
    blinkerSwitchOnTime = 0;
  }

  if (signal != 0)
  {
    if (blinkerSwitchOnTime == 0)
    {
      // Init timer after the first button press detection
      blinkerSwitchOnTime = millis();
    }
    blink(signal);
  }
}

/**
 * Checks the timer whether the turn signal state is on or off.
 */
bool isBlinkerLightOn()
{
  unsigned long elapsed = (millis() - blinkerSwitchOnTime) % turnSignalInterval;
  // return (elapsed < (turnSignalInterval >> 1));
  return true;
}

/**
 * Returns true if we are in the first half of the blinker interval,
 * measured since the blinker was switched on.
 */
bool getBlinkerLightState(unsigned long switchOnTime)
{
  unsigned long elapsed = (millis() - switchOnTime);
  // Less than half the blinker interval elapsed?
  return (elapsed % turnSignalInterval < (turnSignalInterval / 2));
}

void blink(byte side)
{
  //Is the blinker light on or off at the moment?
  bool blinkerState = getBlinkerLightState(blinkerSwitchOnTime);

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

  lastBlinkerLightState = blinkerState;
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
  fill_solid(leds, REAR_LEDS_START, CHSV(32, 20, 8));
  fill_solid(&leds[REAR_LEDS_START], NUM_LEDS - REAR_LEDS_START, CHSV(0, 255, 12));
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
  fill_solid(leds, REAR_LEDS_START, CHSV(0, 0, 15));
  fill_solid(&leds[REAR_LEDS_START], NUM_LEDS - REAR_LEDS_START, CHSV(0, 255, 15));
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
