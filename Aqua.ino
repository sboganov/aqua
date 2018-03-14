#include "Arduino.h"
#include "RTClock.h"
RTClock rt(RTCSEL_LSE); // initialise
USBSerial SerialDebug;

void setup()
{
	SerialDebug.begin(Serial);
}


void loop()
{

}
