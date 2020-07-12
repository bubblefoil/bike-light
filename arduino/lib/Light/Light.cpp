#include "Arduino.h"
#include "Light.h"

LightMode::LightMode(Lights &lights) : _lights(lights)
{
    // _lights = lights;
}

LightMode::~LightMode() {}

void LightMode::updateLights() { Serial.println("Using LightMode base class!"); }
void LightMode::brightnessChange(const unsigned int dt, const int direction)
{
    int increment = (int)dt * _brightnessChangRate * direction;
    if (increment > 0 && _rawBrightness > (_maxBrightness - increment))
    {
        _rawBrightness = _maxBrightness;
    }
    else if (increment < 0 && _rawBrightness < (_minBrightness - increment))
    {
        _rawBrightness = _minBrightness;
    }
    else
    {
        _rawBrightness += increment;
    }
    Serial.print("* + ");
    Serial.print(increment);
    Serial.print(" = ");
    Serial.print(_rawBrightness);
    Serial.print(" / ");
    Serial.println(_maxBrightness);
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

AdaptiveToAmbientLight::AdaptiveToAmbientLight(Lights &lights, uint8_t pinSensor, int numberOfCentralLeds) : LightMode(lights),
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
    int ambient = analogRead(_pinSensor);
    total -= readings[readIndex];
    readings[readIndex] = ambient;
    total += ambient;
    readIndex = (readIndex + 1) % AMBIENT_SMOOTHING_SAMPLES;
    // average = getSmoothedAmbient();
    average = total / AMBIENT_SMOOTHING_SAMPLES;

    // Turn off sides if avg ambient is below threshold calculated from user set brightness.
    byte daylightThreshold = map(_rawBrightness, _minBrightness, _maxBrightness, DAYLIGHT_THRESHOLD_MIN, DAYLIGHT_THRESHOLD_MAX);
    byte sideBrightness = average >= daylightThreshold ? 0 : map(average, 0, 1023, AMBIENT_SIDE_BRIGHTNESS_MIN, AMBIENT_SIDE_BRIGHTNESS_MAX);

#if PRINT_TRACE
    unsigned long now = millis();
    if (now - lastLog > 1000)
    {
        Serial.print("amb=");
        Serial.print(ambient);
        Serial.print(", avg=");
        Serial.print(average);
        Serial.print(", thr=");
        Serial.print(daylightThreshold);
        Serial.print(" -> *");
        Serial.print(sideBrightness);
        Serial.println(" / 255");
        lastLog = now;
    }
#endif

    frontLeft(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    frontRight(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    rearLeft(CHSV(0, 255, sideBrightness), 0, _centralLeds);
    rearRight(CHSV(0, 255, sideBrightness), 0, _centralLeds);

    frontLeft(CHSV(0, 0, 255), _lights.flCnt - _centralLeds);
    frontRight(CHSV(0, 0, 255), _lights.frCnt - _centralLeds);
    rearLeft(CHSV(0, 255, 255), _lights.blCnt - _centralLeds);
    rearRight(CHSV(0, 255, 255), _lights.brCnt - _centralLeds);
}

unsigned int AdaptiveToAmbientLight::getSmoothedAmbient()
{
    // Update floating avarage
    int ambient = analogRead(_pinSensor);
    total -= readings[readIndex];
    readings[readIndex] = ambient;
    total += ambient;
    readIndex = (readIndex + 1) % AMBIENT_SMOOTHING_SAMPLES;
    return total / AMBIENT_SMOOTHING_SAMPLES;
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
    int halfBrightness = (_maxBrightness - _minBrightness) / 2;
    if (_rawBrightness < halfBrightness)
    {
        centralBrightness = map(_rawBrightness, _minBrightness, halfBrightness, 10, 255);
        sideBrightness = 0;
    }
    else
    {
        centralBrightness = 255;
        sideBrightness = map(_rawBrightness, halfBrightness, _maxBrightness, 0, 255);
    }

    // Serial.print("Manual light: ");
    // Serial.print(centralBrightness);
    // Serial.print(",");
    // Serial.println(sideBrightness);

    frontLeft(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    frontRight(CHSV(0, 0, sideBrightness), 0, _centralLeds);
    rearLeft(CHSV(0, 255, sideBrightness), 0, _centralLeds);
    rearRight(CHSV(0, 255, sideBrightness), 0, _centralLeds);

    frontLeft(CHSV(0, 0, centralBrightness), _lights.flCnt - _centralLeds);
    frontRight(CHSV(0, 0, centralBrightness), _lights.frCnt - _centralLeds);
    rearLeft(CHSV(0, 255, centralBrightness), _lights.blCnt - _centralLeds);
    rearRight(CHSV(0, 255, centralBrightness), _lights.brCnt - _centralLeds);
}

ParkingLights::ParkingLights(Lights &lights) : LightMode(lights) {}
ParkingLights::~ParkingLights() {}
void ParkingLights::updateLights()
{
    byte brightness = (byte)map(_rawBrightness, _minBrightness, _maxBrightness, 1, 31);
    frontLeft(CHSV(0, 0, brightness));
    frontRight(CHSV(0, 0, brightness));
    rearLeft(CHSV(0, 255, brightness));
    rearRight(CHSV(0, 255, brightness));
}

PowerSavingLight::PowerSavingLight(Lights &lights, unsigned int blinkOnMillis, unsigned int blinkOffMillis, int numberOfCentralLeds) : LightMode(lights),
                                                                                                                       _blinkOnMillis(blinkOnMillis),
                                                                                                                       _blinkOffMillis(blinkOffMillis),
                                                                                                                       _centralLeds(numberOfCentralLeds)
{
}

PowerSavingLight::~PowerSavingLight()
{
}

void PowerSavingLight::updateLights()
{
    unsigned long t = (millis() - time) % (_blinkOnMillis + _blinkOffMillis);
    byte brightness = t >= _blinkOnMillis ? (byte)map(_rawBrightness, _minBrightness, _maxBrightness, 1, 31) : 0;
    CHSV colF = CHSV(0, 0, brightness);
    CHSV colR = CHSV(0, 255, brightness);
    CHSV off = CHSV(0, 0, 0);

    frontLeft(off, 0, _centralLeds);
    frontRight(off, 0, _centralLeds);
    rearLeft(off, 0, _centralLeds);
    rearRight(off, 0, _centralLeds);

    frontLeft(colF, _lights.flCnt - _centralLeds);
    frontRight(colF, _lights.frCnt - _centralLeds);
    rearLeft(colR, _lights.blCnt - _centralLeds);
    rearRight(colR, _lights.brCnt - _centralLeds);
}
