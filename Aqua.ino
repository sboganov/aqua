#include "RTClock.h"
#include "LightControl.h"
#include <stdio.h>
#include <libmaple/adc.h>
#include <libmaple/iwdg.h>

#define LED_PIN PC13

RTClock rt(RTCSEL_LSE); // initialise
tm_t alarmTime;

#define START_HOUR  8
#define END_HOUR   20

LightControl lightcontrol;

void printTime(tm_t &t) {
	Serial.print(t.hour);
	Serial.print(":");
	Serial.print(t.minute);
	Serial.print(":");
	Serial.print(t.second);
	Serial.print(" day:");
	Serial.print(t.day);
	Serial.print("/");
	Serial.print(t.month);
	Serial.print("/");
	Serial.println(t.year + 1970);
}

void setup_vdd_tempr_sensor() {
	adc_reg_map *regs = ADC1->regs;
	regs->CR2 |= ADC_CR2_TSVREFE;    // enable VREFINT and Temperature sensor
	// sample rate for VREFINT ADC channel and for Temperature sensor
	regs->SMPR1 |= (0b111 << 18);  // sample rate temperature
	regs->SMPR1 |= (0b111 << 21);  // sample rate vrefint
	adc_calibrate(ADC1);
}

void everySecond() {
	digitalWrite(LED_PIN, !digitalRead(LED_PIN));
	iwdg_feed();

	tm_t currentTime;
	rt.getTime(currentTime);
	lightcontrol.update(currentTime);

}

void setup() {
	pinMode(LED_PIN, OUTPUT);
	pinMode(RELAY_PIN, OUTPUT);

	pinMode(PA2, OUTPUT);
	pinMode(PA3, OUTPUT);
	pinMode(PA5, OUTPUT);
	digitalWrite(PA2, LOW);
	digitalWrite(PA3, HIGH);
	digitalWrite(PA5, HIGH);

	for (int i = 0; i < 20; i++) {
		digitalWrite(LED_PIN, HIGH);
		delay(100);
		digitalWrite(LED_PIN, LOW);
		delay(100);
	}

	// Serial.begin();

	Serial.println("              SETUP           ");
	static tm_t ntime;

	if (rt.getTime() < 1000) {
		Serial.println("set up time");
		ntime.day = 02;
		ntime.month = 04;
		ntime.year = 2018 - 1970;
		ntime.hour = 15;
		ntime.minute = 35;
		rt.setTime(ntime);
	}

	setup_vdd_tempr_sensor();

	Serial.println("              INIT           ");
	delay(1000);

	rt.getTime(ntime);

	Serial.println("Setup time");
	delay(1000);

	lightcontrol.init(ntime);
	printTime(ntime);
	Serial.println("watchdog");
	delay(1000);
	iwdg_init(IWDG_PRE_256, 1250); // init an 8 second wd timer
	rt.attachSecondsInterrupt(everySecond);
}

void loop() {
	// delay(5000);
	float tempr, vdd;
	// reading Vdd by utilising the internal 1.20V VREF
	vdd = 1.20 * 4096.0 / adc_read(ADC1, 17);
	// following 1.43 and 0.0043 parameters come from F103 datasheet - ch. 5.9.13
	// and need to be calibrated for every chip (large fab parameters variance)
	tempr = (1.43 - (vdd / 4096.0 * adc_read(ADC1, 16))) / 0.0043 + 25.0;
	Serial.print("Vdd=  ");
	Serial.print(vdd);
	Serial.print(" V ");
	Serial.print("Temp= ");
	Serial.print(tempr);
	Serial.println(" C");

	digitalWrite(PA5, LOW);
	digitalWrite(PA3, HIGH);
	delay(100);
	for (long step = 0; step < 40000; step++) {
		digitalWrite(PA2, HIGH);
		delayMicroseconds(900);
		digitalWrite(PA2, LOW);
		delayMicroseconds(900);
		if (step % 100 == 0) {
			Serial.println(step);
		}
	}
	for (int st = 0; st < 50; st++) {
		digitalWrite(PA5, LOW);
		delay(1000);
		for (int i = 0; i < 2000; i++) {
			digitalWrite(PA2, HIGH);
			delayMicroseconds(900);
			digitalWrite(PA2, LOW);
			delayMicroseconds(900);
		}
		digitalWrite(PA5, HIGH);
		Serial.println(st);
		delay(30000);
	}

	digitalWrite(PA5, LOW);
	digitalWrite(PA3, LOW);
	delay(100);
	for (long step = 0; step < 40000; step++) {
		digitalWrite(PA2, HIGH);
		delayMicroseconds(900);
		digitalWrite(PA2, LOW);
		delayMicroseconds(900);
	}
	digitalWrite(PA5, HIGH);
	delay(1000);
}
