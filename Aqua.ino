#include "Arduino.h"
#include "RTClock.h"

#define LED_PIN PC13

RTClock rt(RTCSEL_LSE); // initialise
USBSerial SerialDebug;

void everySecond() {
	digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void setup()
{
	SerialDebug.begin(Serial);
	pinMode(LED_PIN, OUTPUT);
	rt.attachSecondsInterrupt(everySecond);
}


void loop()
{
	delay(5000);
}
