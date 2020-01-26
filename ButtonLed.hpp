/*
 * ButtonLed.hpp
 *
 *  Created on: 26.01.2020
 *      Author: nick
 *
 * This module is for settings the button LED PWM value.
 *
 * The used buttons have built-in LEDs which can be directly
 * supplied with 5V and my board has a simple FET circuit
 * that can switch the LEDs on an off and also regulate
 * brightness by applying a PWM to it.
 *
 * To enable the module it is enough to define the
 * PIN_BUTTON_LED_PWM in the platformConfig.h file.
 * If disabled the calls are just empty and will
 * be thrown out by the optimizer.
 *
 * ATTENTION: The used pin must be one of the Arduinos PWM pins.
 *
 * TODO: regulate the brightness depending on the measurements
 *       from the also attached light sensing module OPT101.
 *       For now we just set and keep the default brightness.
 */

#ifndef BUTTONLED_HPP_
#define BUTTONLED_HPP_

#include "platformConfig.h"
#include <Arduino.h>

namespace ButtonLed {

// define the defaults if not given by platformConfig.h
#ifndef PIN_BUTTON_LED_PWM_DEFAULT
#defined PIN_BUTTON_LED_PWM_DEFAULT  	50
#endif

#if defined(PIN_BUTTON_LED_PWM)

static inline void set(uint8_t dc) {
	if (dc > 0 && dc < 255)
		analogWrite(PIN_BUTTON_LED_PWM, dc);
	else {
		pinMode(PIN_BUTTON_LED_PWM, OUTPUT);
		digitalWrite(PIN_BUTTON_LED_PWM, dc == 0 ? LOW : HIGH);
	}
}

static inline void loop() {
	// TODO: get light intensity, maybe do some filtering and set PWM accordingly
}

static inline void setup() {
	pinMode(PIN_BUTTON_LED_PWM, OUTPUT);
	// switch the circuit on during the startup phase
	set(PIN_BUTTON_LED_PWM_DEFAULT);
}

#else
static inline void set(uint_8_t) {};
static inline void loop() {};
static inline void setup() {};
#endif

} // END namespace ButtonLed

#endif /* BUTTONLED_HPP_ */
