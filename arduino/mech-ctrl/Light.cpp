#include "Arduino.h"
#include "Light.h"

LightMode::LightMode(Lights &lights) : _lights(lights)
{
    // _lights = lights;
}

LightMode::~LightMode() {}

void LightMode::updateLights() { Serial.println("Using LightMode base class!"); }
void LightMode::brightnessChange(const unsigned int dt, const BrightnessChange change)
{
    _rawBrightness += (dt * change);
}

void LightMode::frontLeft(const CHSV &col, int startOffset /*=0*/, int endOffset /*=0*/)
{
    fill_solid(&_lights.fl[startOffset], _lights.flCnt - startOffset - endOffset, col);
}

void LightMode::frontRight(const CHSV &col, int startOffset /*=0*/, int endOffset /*=0*/)
{
    fill_solid(&_lights.fr[startOffset], _lights.frCnt - startOffset - endOffset, col);
}

void LightMode::rearLeft(const CHSV &col, int startOffset /*=0*/, int endOffset /*=0*/)
{
    fill_solid(&_lights.bl[startOffset], _lights.blCnt - startOffset - endOffset, col);
}

void LightMode::rearRight(const CHSV &col, int startOffset /*=0*/, int endOffset /*=0*/)
{
    fill_solid(&_lights.br[startOffset], _lights.brCnt - startOffset - endOffset, col);
}

LightsOff::LightsOff(Lights &lights) : LightMode(lights)
{
}

LightsOff::~LightsOff()
{
}

void LightsOff::updateLights()
{
    // Serial.println("Lights off");
    FastLED.clear(false);
}

AdaptiveToAmbientLight::AdaptiveToAmbientLight(Lights &lights, unsigned int pinSensor, int numberOfCentralLeds) : LightMode(lights),
                                                                                                                  _pinSensor(pinSensor),
                                                                                                                  _centralLeds(numberOfCentralLeds)
{
    pinMode(pinSensor, INPUT);
}

AdaptiveToAmbientLight::~AdaptiveToAmbientLight()
{
}

void AdaptiveToAmbientLight::updateLights()
{
    byte sideBrightness = analogRead(_pinSensor) >> 3; // 1/8 * 1024
    // Serial.print("Adaptive light: ");
    // Serial.println(sideBrightness);
    frontLeft(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    frontLeft(CHSV(0, 0, 255), _lights.flCnt - _centralLeds);
    frontRight(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    frontRight(CHSV(0, 0, 255), _lights.frCnt - _centralLeds);
    rearLeft(CHSV(0, 255, sideBrightness), 0, _centralLeds);
    rearLeft(CHSV(0, 255, 255), _lights.blCnt - _centralLeds);
    rearRight(CHSV(0, 255, sideBrightness), 0, _centralLeds);
    rearRight(CHSV(0, 255, 255), _lights.brCnt - _centralLeds);
}

ManualBrightnessLight::ManualBrightnessLight(Lights &lights, int numberOfCentralLeds) : LightMode(lights),
                                                                                        _centralLeds(numberOfCentralLeds)
{
}

ManualBrightnessLight::~ManualBrightnessLight()
{
}

void ManualBrightnessLight::updateLights()
{
    byte centralBrightness;
    byte sideBrightness;
    unsigned int halfBrightness = (_maxBrightness - _minBrightness);
    if (_rawBrightness < halfBrightness)
    {
        centralBrightness = map(_rawBrightness, halfBrightness, _maxBrightness, 10, 255);
        sideBrightness = 0;
    }
    else
    {
        centralBrightness = 255;
        sideBrightness = map(_rawBrightness, halfBrightness, _minBrightness, 10, 255);
    }
    // Serial.print("Manual light: ");
    // Serial.print(centralBrightness);
    // Serial.print(",");
    // Serial.println(sideBrightness);

    frontLeft(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    frontRight(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    rearLeft(CHSV(0, 255, sideBrightness), 0, _centralLeds);
    rearRight(CHSV(0, 255, sideBrightness), 0, _centralLeds);

    frontLeft(CHSV(0, 0, centralBrightness),  _lights.flCnt - _centralLeds);
    frontRight(CHSV(0, 0, centralBrightness),  _lights.frCnt - _centralLeds);
    rearLeft(CHSV(0, 255, centralBrightness),  _lights.blCnt - _centralLeds);
    rearRight(CHSV(0, 255, centralBrightness),  _lights.brCnt - _centralLeds);
}

ParkingLights::ParkingLights(Lights &lights) : LightMode(lights) {}
ParkingLights::~ParkingLights() {}
void ParkingLights::updateLights()
{
    frontLeft(CHSV(0, 0, 10));
    frontRight(CHSV(0, 0, 10));
    rearLeft(CHSV(0, 255, 10));
    rearRight(CHSV(0, 255, 10));
}