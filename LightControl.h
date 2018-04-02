/*
 * LightControl.h
 *
 *  Created on: 24 мар. 2018 г.
 *      Author: sboga
 */

#ifndef LIGHTCONTROL_H_
#define LIGHTCONTROL_H_
#include "RTClock.h"
#define RELAY_PIN	PA4


class LightControl {
public:
	LightControl() {

	}
void init(tm_t &currentTime) {
	if (currentTime.hour >= 8 && currentTime.hour < 20) {
		digitalWrite(RELAY_PIN, LOW);
	} else {
		digitalWrite(RELAY_PIN, HIGH);
	}
}

void update(tm_t &currentTime) {
	init(currentTime);
}
};

#endif /* LIGHTCONTROL_H_ */
