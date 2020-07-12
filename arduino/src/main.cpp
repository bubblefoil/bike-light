#include <Arduino.h>
#include <FastLED.h>
#include "Button.h"
#include "Light.h"

#define PRINT_DEBUG 1
#define PRINT_TRACE 0

#if PRINT_DEBUG
#define PRINTS(s)       \
  {                     \
    Serial.print(F(s)); \
  }
#define PRINT(s, v)     \
  {                     \
    Serial.print(F(s)); \
    Serial.print(v);    \
  }
#define PRINTX(s, x)      \
  {                       \
    Serial.print(F(s));   \
    Serial.print(x, HEX); \
  }
#else
#define PRINTS(s)
#define PRINT(s, v)
#define PRINTX(s, x)
#endif

// Total number of LEDs in the strip
#define NUM_LEDS 36
// Control the LED strips
#define DATA_PIN_BR 2
#define DATA_PIN_BL 3
#define DATA_PIN_FL 4
#define DATA_PIN_FR 5

// TBD - Bluetooth communication
#define RX 6
#define TX 7

// Digital input pins for buttons
#define pinLightMode 11
#define pinTurnLeft 10
#define pinTurnRight 12

// Size of battery voltage smoothing buffer
#define BAT_SAMPLES 16
// Period in millis between taking battery voltage samples
#define BAT_SAMPLE_INTRVAL 255

const uint8_t pinPhotoResistor = A0;
const uint8_t pinBatteryVoltage = A4;

// Measure the actual resistance of divider resistors.
// Note: If there is a chance of voltage on the analog pin when Arduino is not powered,
// make sure R1 is large enough that current into the pin is < 10 µA and there is a 100 nF capacitor which charges Arduino's internal capacitor fast enough during sampling.
// Otherwise, go with R1 ~ 10k
const float batteryVoltageDividerR1 = 10000000;
const float batteryVoltageDividerR2 = 2000000;
// Measure the actual voltage on the board's 5V pin
const float boardReferenceVoltage = 4.995;
// Voltages of battery states for charge indication.
const float batteryVoltageFull = 14.0;
const float batteryVoltageNominal = 12.0;
const float batteryVoltageCritical = 9.0;
const int N_LED_BATTERY_STATUS = 12;
const byte BATTERY_STATUS_HUE = 108;

// Size of battery voltage smoothing buffer
float batteryVoltageSamples[BAT_SAMPLES];
byte batteryVoltageReadingIndex = 0;

// The light mode which indecates battery status for a moment after being activated
const byte BATTERY_CHARGE_INDICATOR_MODE = 3;
// Time of the last light mode change. Used by battery indicator, which is active for a while after switching to BATTERY_CHARGE_INDICATOR_MODE
unsigned long lastModeChangeTime = 0;

// Currently selected light mode. Set to OFF by default mainly for development because if MCU is powered from USB only, strips would try to draw excessive power from it.
byte lightMode = 0;

// Millis until button press is interpreted as holding.
const unsigned int holdThresholdTime = 300;
// Last mode button hold duration. Used to find time increment for gradual brightness change.
unsigned int lastHoldTime = 0;
// Sign of the brightness increment. Flipped every time the button is held and released. Do not use byte, it is unsigned!
int brightnessChangeDirection = 1;

// Left blinker bit
const byte LEFT = 1 << 0;
// Right blinker bit
const byte RIGHT = 1 << 1;
// Blinker light hue in the range from 0 - red to 255 - red again. Orange should be 32 but it seemed too yellowish.
const byte BLINKER_HUE = 22;
// Blinker minimum light brightness in on the sides. Gradiently brighter towords center. Range 0 - 255, although below 30 the color looks horrible and below 25 it turns red.
const byte BLINKER_DIM_BRIGHTNESS = 40;
// Total duration of a single ON/OFF blink in milliseconds.
const unsigned long BLINKER_INTERVAL = 800L;
// Hazard warning lights gesture period in milliseconds. If a blinker is turned on 3-times withing this period, lights switch to Harard warning light mode.
const unsigned long HAZARD_LIGHTS_GESTURE_TIME = 800L;
// Last timestamps of turning on blinker. Used to detect 3 quick changes which actvate warning lights.
unsigned long blinkerTurnOnTimes[] = {-4 * HAZARD_LIGHTS_GESTURE_TIME, -2 * HAZARD_LIGHTS_GESTURE_TIME, -HAZARD_LIGHTS_GESTURE_TIME}; // Init to something that won't trigger the gesture right away.

// Number of head/tail light LEDs. These shine at full brightness.
const int N_FRONTAL = 6;
// Number of head/tail light LEDs in power saving mode.
const int N_PWR_SAVING = 10;

// Memory for all LED colors
// Front left LED colors
CRGB ledsFL[NUM_LEDS];
// Front right LED colors
CRGB ledsFR[NUM_LEDS];
// Rear left LED colors
CRGB ledsBL[NUM_LEDS];
// Rear right LED colors
CRGB ledsBR[NUM_LEDS];

//LED arrays
Lights lights = Lights(ledsFL, ledsFR, ledsBL, ledsBR, NUM_LEDS, NUM_LEDS, NUM_LEDS, NUM_LEDS);

// Individual light modes.
// Classes implement abstract class LightMode with updateLights and changeBrightness methods.
LightsOff lightsOff = LightsOff(lights);
AdaptiveToAmbientLight adaptiveToAmbientLight = AdaptiveToAmbientLight(lights, pinPhotoResistor, N_FRONTAL);
ManualBrightnessLight manualBrightnessLight = ManualBrightnessLight(lights, N_FRONTAL);
ParkingLights parkingLights = ParkingLights(lights);
PowerSavingLight powerSavingLight = PowerSavingLight(lights, 300, 200, N_PWR_SAVING);

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
    &powerSavingLight};

//Temporary variable to test brightness control.
byte globalBrightness = 255;

// Controls light mode. Holding the button changes brightness.
Button buttonLightMode = Button(pinLightMode, BUTTON_PULLUP_INTERNAL, true, 50);
Button buttonBlinkerLeft = Button(pinTurnLeft, BUTTON_PULLUP_INTERNAL, true, 50);
Button buttonBlinkerRight = Button(pinTurnRight, BUTTON_PULLUP_INTERNAL, true, 50);

// Forward prototype declaration because of the default parameter.
void updateLights(bool show = true);

// Reads raw analog value and calculates voltage from divider values
float readBatteryVoltage();
// Reads battery voltage and adds new sample to a circular buffer
void takeBatteryVoltageSample();
// Calculates avarage voltage from the buffer of samples
float getSmoothedBatteryVoltage();
// Fills buffer with voltage samples
void initBatteryVoltageStatus();
// Shows battery voltage by LEDs
void indicateBatteryVoltageStatus(float voltage);

void handleBlinkers();
void blink(unsigned int blinkerOnTime, byte side);
void frontLeft(const CHSV &col);
void frontRight(const CHSV &col);
void rearLeft(const CHSV &col);
void rearRight(const CHSV &col);
void frontalArea();

/**
 * Changes light mode to the next one, in a circle.
 */
void nextLightMode()
{
  lastModeChangeTime = millis();
  lightMode++;
  lightMode %= (sizeof(lightModes) / sizeof(LightMode *));
  Serial.print("Light mode: ");
  Serial.println(lightMode);
  updateLights();
}

void nextLightModeHandler(Button &b)
{
  nextLightMode();
  PRINT("Battery: ", getSmoothedBatteryVoltage());
}

/**
 * Release button handler which is active during holding event.
 * When the button is release after a hold event, the release hanlder is set back to its normal light mode change function.
 * Also the brightness change direction is flipped so when the button is held again, it makes the opposite change of brightness.
 */
void activateLightModeChangeHandler(Button &b)
{
  brightnessChangeDirection *= -1;
  b.releaseHandler(nextLightModeHandler);
  PRINT("*^ ", brightnessChangeDirection);
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

void printAmbientLightLevel()
{
#if PRINT_DEBUG
  Serial.print("Light: ");
  Serial.print(analogRead(pinPhotoResistor));
  Serial.println(" / 1023");
  indicateBatteryVoltageStatus(getSmoothedBatteryVoltage());
#endif
}

void blinkerPressed(Button &b)
{
  // Write down the time of this blinker button press
  blinkerTurnOnTimes[0] = blinkerTurnOnTimes[1];
  blinkerTurnOnTimes[1] = blinkerTurnOnTimes[2];
  blinkerTurnOnTimes[2] = millis();

#if PRINT_TRACE
  Serial.print("blinker press times:");
  Serial.print(blinkerTurnOnTimes[0]);
  Serial.print("ms, ");
  Serial.print(blinkerTurnOnTimes[1]);
  Serial.print("ms, ");
  Serial.print(blinkerTurnOnTimes[2]);
  Serial.print("ms, time=");
  Serial.print(blinkerTurnOnTimes[2] - blinkerTurnOnTimes[0]);
  Serial.println("ms");
#endif

  printAmbientLightLevel();
}

void processSerialInput()
{
  // FIXME: Serial input is busy when FastLED writes to strips.
  // https://github.com/FastLED/FastLED/wiki/Interrupt-problems
  while (Serial.available() > 0)
  {
    int receivedText = Serial.read();
    Serial.println(receivedText);
    char state = receivedText;
    // FIXME: Convert to byte before comparing to number of light modes
    if (state >= '0' && state <= '3')
    {
      lightMode = (byte)(state - '0');
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
  buttonBlinkerLeft.pressHandler(blinkerPressed);
  buttonBlinkerRight.pressHandler(blinkerPressed);

  FastLED.addLeds<NEOPIXEL, DATA_PIN_FL>(ledsFL, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_FR>(ledsFR, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_BL>(ledsBL, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_BR>(ledsBR, NUM_LEDS);

  initBatteryVoltageStatus();
  indicateBatteryVoltageStatus(getSmoothedBatteryVoltage());

  updateLights();
}

/**
 * Main Arduino's infinite loop.
*/
void loop()
{
  // Let Button library check button state
  buttonLightMode.process();
  // Handle communication. Currently disabled until BT communication is figured out.
  // void processSerialInput();
  // Sample batter voltage to keep track of running avarage.
  void takeBatteryVoltageSample();

  // Handle long press of mode change and update raw brightness value of selected light mode.
  // Note: Cannot use heldFor() because it checks whether hold event has already been fired. Use better timing here.
  unsigned int holdTime = buttonLightMode.holdTime();
  if (buttonLightMode.heldFor(holdThresholdTime) && holdTime > lastHoldTime)
  {
    lightModes[lightMode]->brightnessChange(holdTime - lastHoldTime, brightnessChangeDirection);
    // Keep track of the time since the last change so we can send delta time and brightness can change at constant speed.
    lastHoldTime = holdTime;
  }
  else
  {
    lastHoldTime = holdThresholdTime;
  }

  // Call the update lights method of selected mode implementation
  updateLights(false);
  // If parking lights have been selected for less then certain period, signal battery charge level by LEDs.
  if (lightMode == BATTERY_CHARGE_INDICATOR_MODE && millis() - lastModeChangeTime < 2000)
  {
    indicateBatteryVoltageStatus(getSmoothedBatteryVoltage());
  }
  // After lights set their colors, override them by blinker if needed.
  handleBlinkers();

  // Send new values to LEDs and wait for the next time frame, using FastLED delay to enable time dithering if its ever used.
  FastLED.show();
  FastLED.delay(10);
}

void handleBlinkers()
{
  // Check if hazard warning lights should be activated by gesture
  unsigned long lastGestureTime = blinkerTurnOnTimes[2] - blinkerTurnOnTimes[0];
  byte hazard = (lastGestureTime <= HAZARD_LIGHTS_GESTURE_TIME) ? LEFT | RIGHT : 0;

  if (buttonBlinkerLeft.isPressed())
  {
    blink(buttonBlinkerLeft.holdTime(), LEFT | hazard);
  }
  // Not else if to enable using HW which can activate hazard lights by pressing both blinker buttons.
  if (buttonBlinkerRight.isPressed())
  {
    blink(buttonBlinkerRight.holdTime(), RIGHT | hazard);
  }
}

/**
 * Returns true if we are in the first half of the blinker interval,
 * measured since the blinker was switched on.
 */
bool getBlinkerLightState(unsigned int switchOnTime)
{
  // Less than half the blinker interval elapsed?
  return (switchOnTime % BLINKER_INTERVAL < (BLINKER_INTERVAL / 2));
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
  CHSV blinkerColorFull = CHSV(BLINKER_HUE, 255, brightness);
  CHSV blinkerColorDim = CHSV(BLINKER_HUE, 255, brightness & BLINKER_DIM_BRIGHTNESS);
  if (side & RIGHT)
  {
    fill_gradient_RGB(ledsFR, NUM_LEDS, blinkerColorDim, blinkerColorDim, blinkerColorDim, blinkerColorFull);
    fill_gradient_RGB(ledsBR, NUM_LEDS, blinkerColorDim, blinkerColorDim, blinkerColorDim, blinkerColorFull);
  }
  if (side & LEFT)
  {
    fill_gradient_RGB(ledsFL, NUM_LEDS, blinkerColorDim, blinkerColorDim, blinkerColorDim, blinkerColorFull);
    fill_gradient_RGB(ledsBL, NUM_LEDS, blinkerColorDim, blinkerColorDim, blinkerColorDim, blinkerColorFull);
  }
}

// Calls updateLights method of currently active light mode. If show=true, writes color values to LEDs.
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

float readBatteryVoltage()
{
  // Take more samples and drop the first one. Return AVG.
  analogRead(pinBatteryVoltage);
  int inputValue = 0;
  for (byte i = 0; i < 16; i++)
  {
    inputValue += analogRead(pinBatteryVoltage);
  }
  float pinInputVolts = ((inputValue >> 4) * boardReferenceVoltage) / 1024.0;
  float batteryVolts = pinInputVolts / (batteryVoltageDividerR2 / (batteryVoltageDividerR1 + batteryVoltageDividerR2));
  return batteryVolts;
}

void takeBatteryVoltageSample()
{
  batteryVoltageSamples[batteryVoltageReadingIndex] = readBatteryVoltage();
  batteryVoltageReadingIndex = (batteryVoltageReadingIndex + 1) % BAT_SAMPLES;
}

void initBatteryVoltageStatus()
{
  readBatteryVoltage();
  for (byte i = 0; i < BAT_SAMPLES; i++)
  {
    batteryVoltageSamples[i] = readBatteryVoltage();
  }
}

float getSmoothedBatteryVoltage()
{
  float total = 0;
  for (byte i = 0; i < BAT_SAMPLES; i++)
  {
    total += batteryVoltageSamples[i];
  }
  float voltage = total / (float)BAT_SAMPLES;
  return voltage;
}

void indicateBatteryVoltageStatus(float voltage)
{
  // Extra handling of discharged or fully charged battery. Otherwise, calculate anti-aliassed color bar.
  if (voltage < batteryVoltageCritical)
  {
    fill_solid(&ledsFL[0], 1, CRGB::Red);
    fill_solid(&ledsFR[0], 1, CRGB::Red);
  }
  else if (voltage >= batteryVoltageFull)
  {
    CHSV col = CHSV(BATTERY_STATUS_HUE, 255, 255);
    fill_solid(&ledsFL[0], N_LED_BATTERY_STATUS, col);
    fill_solid(&ledsFR[0], N_LED_BATTERY_STATUS, col);
  }
  else
  {
    // Charge percentage, 0.0 - 1.0
    float percent = (batteryVoltageFull - batteryVoltageCritical) / voltage;
    float ledsOnDec = percent * N_LED_BATTERY_STATUS;        // Number of indicator LEDs to light up with decimal part
    float ledsFull = floor(ledsOnDec);                       // Number of LEDs to light up at full brightness as float
    int ledsFullInt = (int)ledsFull;                         // Number of LEDs at 100% brightness as int
    byte decimal = (byte)(255.0 * (ledsOnDec - ledsFull));   // The decimal part shown by a dimmer LED
    CHSV colDimmer = CHSV(BATTERY_STATUS_HUE, 255, decimal); // Color of the dimmer LED at the On/Off boundary
    CHSV col = CHSV(BATTERY_STATUS_HUE, 255, 255);           // Color of battery charge indicator LEDs
    // Apply colors
    fill_solid(&ledsFL[0], ledsFullInt, col);
    fill_solid(&ledsFR[0], ledsFullInt, col);
    ledsFL[ledsFullInt] = colDimmer;
    ledsFR[ledsFullInt] = colDimmer;
    // Turn off rest of the lights to show full length of the indicator bar
    if (ledsFullInt < N_LED_BATTERY_STATUS)
    {
      fill_solid(&ledsFL[ledsFullInt + 1], N_LED_BATTERY_STATUS - ledsFullInt, CRGB::Black);
      fill_solid(&ledsFR[ledsFullInt + 1], N_LED_BATTERY_STATUS - ledsFullInt, CRGB::Black);
    }
  }
}
