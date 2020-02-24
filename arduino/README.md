# Lights control unit software

Arduino application / LED strip driver.
Supplies data for WS2812B based addressable LED strips.
Uses 3 buttons as input, one for selecting light mode and changing brightness, the other two activate turn signals.

Additionally, the unit is prepared to measure ambient light by a photoresistor and battery voltage level.

It is planned to decode data on serial input as commands so the unit may be controlled via HT-05 bluetooth module. This will require solving the conflict with FastLED, which disables interrupts during feeding the strips and incoming serial data are lost.

## Development

Application is developed with [PlatformIO](https://platformio.org/platformio-ide), which has better dependency management and works with clean C++ code instead of special .ino files.

## TODO List

* Ligths off when blinking in every mode
* Implement power saving mode - blinking
* Switch to power saving mode on low battery voltage
* Less bright blinkers in power saving mode
* Fix Serial issue #2
* Animate blinkers
* Animate light changes
