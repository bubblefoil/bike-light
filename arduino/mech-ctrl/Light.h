#ifndef Light_h
#define Light_h

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
 * Direction of the brightness change.
*/
enum BrightnessChange
{
    MORE = -1,
    LESS = 1
};

/*
 * Abstract class for a light mode. Subclasses must implement the updateLights() method, which updates LED values.
*/
class LightMode
{
protected:
    Lights &_lights;
    const unsigned int _minBrightness = 0;
    const unsigned int _maxBrightness = 0xFFFF;
    //Updated by the brightness button. Each mode may interpret this value as needed.
    unsigned int _rawBrightness = 0x7FFF;

public:
    LightMode(Lights &lights);
    ~LightMode();
    virtual void updateLights();
    //
    virtual void brightnessChange(const unsigned int dt, const BrightnessChange change);
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
    unsigned int _pinSensor;
    int _centralLeds;

public:
    AdaptiveToAmbientLight(Lights &lights, unsigned int pinSensor, int numberOfCentralLeds);
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