#ifndef Light_h
#define Light_h 

#include <FastLED.h>

class LightMode
{
private:
    // CRGB[] leds;
public:
    LightMode(byte numLeds);
    ~LightMode();
    // void updateLights(CRGB[] fl, CRGB[] fr, CRGB[] bl, CRGB[] br);
    // void updateLights(CRGB[] fl, CRGB[] fr, CRGB[] bl, CRGB[] br);
};

LightMode::LightMode(byte numLeds)
{
}

LightMode::~LightMode()
{
}

#endif