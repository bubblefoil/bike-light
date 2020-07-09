#ifndef Light_h
#define Light_h

#define MORE 1
#define LESS -1

// Ambient light is sampled every loop. Smooting period depends on the duration of every cycle.
// Because sum of whole LIFO must be calculated, there is a limit of samples:
// sample count * max analogRead value <= max unsigned int
// 
#define AMBIENT_SMOOTHING_SAMPLES 255
// Manually set brightness will be mapped to this range
// and the result will be compared to actual abmient light to decide whether sides should shine or not.
// Should be between 0 - 1023
#define DAYLIGHT_THRESHOLD_MIN 50
#define DAYLIGHT_THRESHOLD_MAX 800
// If ambient light is below daylignt threshold, side will shine at brightness within this range.
#define AMBIENT_SIDE_BRIGHTNESS_MIN 4
#define AMBIENT_SIDE_BRIGHTNESS_MAX 127

#define PRINT_DEBUG 1

#include <Arduino.h>
#include <FastLED.h>

/**
 * Contains the LED arrays and their lengths for each corner's light.
 */
struct Lights
{
    CRGB *fl;
    const int flCnt;
    CRGB *fr;
    const int frCnt;
    CRGB *bl;
    const int blCnt;
    CRGB *br;
    const int brCnt;

    Lights(CRGB flLeds[], CRGB frLeds[], CRGB blLeds[], CRGB brLeds[],
           int flCount, int frCount, int blCount, int brCount) : flCnt(flCount), frCnt(frCount), blCnt(blCount), brCnt(brCount)
    {
        fl = flLeds;
        fr = frLeds;
        bl = blLeds;
        br = brLeds;
    }
};

/*
 * Abstract class for a light mode. Subclasses must implement the updateLights() method, which updates LED values.
*/
class LightMode
{
protected:
    Lights &_lights;
    const int _minBrightness = 0;
    const int _maxBrightness = 0x7FFF;
    // 1/2048 of full range per ms -> 0-100% in 2s
    const int _brightnessChangRate = 12;
    //Updated by the brightness button. Each mode may interpret this value as needed.
    int _rawBrightness = 0x7FFF >> 1;

public:
    LightMode(Lights &lights);
    ~LightMode();
    virtual void updateLights();
    //
    virtual void brightnessChange(const unsigned int dt, const int direction);
    void frontLeft(const CHSV &col, int startOffset = 0, int endOffset = 0);
    void frontRight(const CHSV &col, int startOffset = 0, int endOffset = 0);
    void rearLeft(const CHSV &col, int startOffset = 0, int endOffset = 0);
    void rearRight(const CHSV &col, int startOffset = 0, int endOffset = 0);
};

class LightsOff : public LightMode
{
public:
    LightsOff(Lights &lights);
    ~LightsOff();
    virtual void updateLights();
};

/**
 * Activates day/night light mode with brightness driven by an ambient light sensor.
*/
class AdaptiveToAmbientLight : public LightMode
{
private:
    uint8_t _pinSensor;
    int _centralLeds;
    // Readings from the analog input
    unsigned int readings[AMBIENT_SMOOTHING_SAMPLES];
    // Index of the current reading
    byte readIndex = 0;
    // Running total
    unsigned long total = 0;
    // Sliding average - smoothed value from analog read (same range)
    unsigned int average = 0;
    // Used for periodic debug prints
    unsigned long lastLog = 0;
    unsigned int getSmoothedAmbient();

public:
    AdaptiveToAmbientLight(Lights &lights, uint8_t pinSensor, int numberOfCentralLeds);
    ~AdaptiveToAmbientLight();
    virtual void updateLights();
};

/**
 * Activates day/night light mode with manual brightness settings.
 * 
 * Dim light on sides, full brightness on nose and tail.
*/
class ManualBrightnessLight : public LightMode
{
private:
    int _centralLeds;

public:
    ManualBrightnessLight(Lights &lights, int numberOfCentralLeds);
    ~ManualBrightnessLight();
    virtual void updateLights();
};

/**
 * Activates parking lights mode. Dim red/white lights all around.
*/
class ParkingLights : public LightMode
{
public:
    ParkingLights(Lights &lights);
    ~ParkingLights();
    virtual void updateLights();
};

#endif