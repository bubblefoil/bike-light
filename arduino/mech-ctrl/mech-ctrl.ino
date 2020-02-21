#include <FastLED.h>
#include "Button.h"
#include "Light.h"

// Total number of LEDs in the strip
#define NUM_LEDS 36
// Control the LED strips
#define DATA_PIN_FL 2
#define DATA_PIN_FR 3
#define DATA_PIN_BL 5
#define DATA_PIN_BR 4

// TBD - Bluetooth communication
#define RX 6
#define TX 7

// Digital input pins for buttons
#define pinLightMode 11
#define pinTurnLeft 10
#define pinTurnRight 12

const int pinPhotoResistor = A0;
const int pinBatteryVoltage = A4;

// Default light mode. Set to OFF mainly for development with just USB power.
const byte OFF = 0;
byte lightMode = OFF;

// Millis until button press is interpreted as holding.
const unsigned int holdThresholdTime = 300;
// Last mode button hold duration. Used to find time increment for gradual brightness change.
unsigned int lastHoldTime = 0;
// Sign of the brightness increment. Flipped every time the button is held and released. Do not use byte, it is unsigned!
int brightnessChangeDirection = 1;

//Left blinker
const byte LEFT = 11;
//Right blinker
const byte RIGHT = 12;
//Blinker light hue in the range from 0 - red to 255 - red again
const byte BLINKER_HUE = 24;
// Total duration of a single ON/OFF blink in milliseconds.
const unsigned long turnSignalInterval = 1000L;

// Number of head/tail light LEDs. These shine at full brightness.
const int N_FRONTAL = 6;
// Number of turn signal LEDs
const byte N_TURN_SIG = 12;

// Memory for all LED values
CRGB ledsFL[NUM_LEDS];
CRGB ledsFR[NUM_LEDS];
CRGB ledsBL[NUM_LEDS];
CRGB ledsBR[NUM_LEDS];

//LED arrays
Lights lights = Lights(ledsFL, ledsFR, ledsBL, ledsBR, NUM_LEDS, NUM_LEDS, NUM_LEDS, NUM_LEDS);

// Individual light modes.
// Classes implement abstract class LightMode with updateLights and changeBrightness methods.
LightsOff lightsOff = LightsOff(lights);
AdaptiveToAmbientLight adaptiveToAmbientLight = AdaptiveToAmbientLight(lights, pinPhotoResistor, N_FRONTAL);
ManualBrightnessLight manualBrightnessLight = ManualBrightnessLight(lights, N_FRONTAL);
ParkingLights parkingLights = ParkingLights(lights);

/** Array of pointers to the light modes. Cycled through when switching modes.
* These have to be pointers, otherwise modes would be stored and used by value, but through a LightMode referece,
* which causes "object slicing" and caling the base method instead of derived.
* https://stackoverflow.com/questions/274626/what-is-object-slicing
*/
LightMode *lightModes[] = {
    &lightsOff,
    &adaptiveToAmbientLight,
    &manualBrightnessLight,
    &parkingLights,
};

// Forward prototype declaration because of the default parameter.
void updateLights(bool show = true);

//Temporary variable to test brightness control.
byte globalBrightness = 255;

// Controls light mode. Holding the button changes brightness.
Button buttonLightMode = Button(pinLightMode, BUTTON_PULLUP_INTERNAL, true, 50);
Button buttonBlinkerLeft = Button(pinTurnLeft, BUTTON_PULLUP_INTERNAL, true, 50);
Button buttonBlinkerRight = Button(pinTurnRight, BUTTON_PULLUP_INTERNAL, true, 50);

/**
 * Changes light mode to the next one, in a circle.
 */
void nextLightMode()
{
  lightMode++;
  lightMode %= (sizeof(lightModes) / sizeof(LightMode *));
  Serial.print("Light mode: ");
  Serial.println(lightMode);
  updateLights();
}

void nextLightModeHandler(Button &b)
{
  nextLightMode();
}

/**
 * Release button handler which is active during holding event.
 * When the button is release after a hold event, the release hanlder is set back to its normal light mode change function.
 * Also the brightness change direction is flipped so when the button is held again, it makes the opposite change of brightness.
 */
void activateLightModeChangeHandler(Button &b)
{
  brightnessChangeDirection *= -1;
  Serial.print("*^ ");
  Serial.println(brightnessChangeDirection);
  b.releaseHandler(nextLightModeHandler);
}

/**
 * When a hold event is triggered, this hold handler changes Button release handler not to change Light mode,
 * because user is changing brightness. 
 */
void activateBrightnessChangeMode(Button &b)
{
  // Deactivate click handler to ignore the last click, triggered by button hold.
  // This handler will reactivate the original light mode change click handler.
  b.releaseHandler(activateLightModeChangeHandler);
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

void processSerialInput()
{
  // FIXME: Serial input is busy when FastLED writes to strips.
  // https://github.com/FastLED/FastLED/wiki/Interrupt-problems
  while (Serial.available() > 0)
  {
    String receivedText = Serial.readString();
    Serial.println(receivedText);
    char state = receivedText[0];
    if (state >= '0' && state < (sizeof(lightModes) / sizeof(LightMode *)))
    {
      lightMode = (byte)state;
    }
    else
    {
      switch (state)
      {
      case '+':
        lightModes[lightMode]->brightnessChange(100, MORE);
        break;
      case '-':
        lightModes[lightMode]->brightnessChange(100, LESS);
        break;
      case 'm':
        nextLightMode();
        break;
      default:
        break;
      }
    }
  }
}

/**
 * Initialization when Arduino is turned on.
*/
void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino ON.");

  pinMode(pinPhotoResistor, INPUT); // Set pinPhotoResistor - A0 pin as an input (optional)

  // Setup buttons
  buttonLightMode.releaseHandler(nextLightModeHandler);
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

/**
 * Main Arduino's infinite loop.
*/
void loop()
{
  buttonLightMode.process();
  void processSerialInput();

  //todo Cannot use heldFor() because it checks whether hold event has already been fired. Use better timing here.
  unsigned int holdTime = buttonLightMode.holdTime();
  if (buttonLightMode.heldFor(holdThresholdTime) && holdTime > lastHoldTime)
  {
    lightModes[lightMode]->brightnessChange(holdTime - lastHoldTime, brightnessChangeDirection);
    lastHoldTime = holdTime;
  }
  else
  {
    lastHoldTime = holdThresholdTime;
  }

  updateLights(false);
  handleBlinkers();
  FastLED.show();
  FastLED.delay(10);
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
  LightMode *mode = lightModes[lightMode];
  mode->updateLights();

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
 * Lights up the frontal area of both head and tail light with white and red, respectively.
*/
void frontalArea()
{
  fill_solid(&ledsFL[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::White);
  fill_solid(&ledsFR[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::White);
  fill_solid(&ledsBL[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::Red);
  fill_solid(&ledsBR[NUM_LEDS - N_FRONTAL], N_FRONTAL, CRGB::Red);
}
