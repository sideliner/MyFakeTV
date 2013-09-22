MyFakeTV
========

My Fake TV - Arduino-based

This is my implementation of FakeTV device, based on an Arduino board. Main components:
- Arduino board (pretty much any board can do, I used Bare Bones Board from Modern Device);
- Toshiba ULN2003 Darlington array;
- bright LEDs (rated forward current 30 mA) - one blue, two green, two red, one white;
- one superbright white LED (rated forward current 30 mA) - to simulate a reading light or such;
- NSI45030AT1G current source drivers, one per LED;
- ldr (photo resistor), 10k pot and a button, to calibrate at what luminosity the lights go on.

The core functions are borrowed from http://forum.arduino.cc/index.php/topic,148967.0.html,
they are modified and expanded to make its action more entertaining and realistic.

Note that my LEDs turned out to be too bright, I would pick 20 mA LEDs (except the superbright one)
if I were to make it again. It is important to cover LEDs with some sort of diffuser.
