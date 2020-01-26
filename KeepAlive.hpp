/*
 * KeepAlive.hpp
 *
 *  Created on: 26.01.2020
 *      Author: nick
 *
 * This module is for regularly triggering the
 * keep-alive-circuit to prevent the used USB
 * power bank from shutting down.
 *
 * This is needed because my power bank seems to
 * have quite a high shutdown limit of a bit more
 * than 120mA.
 *
 * Depending on the current system state (playing,
 * paused, ...) my whole system consumes less than
 * that.
 *
 * To overcome this I added a circuit that, if
 * enabled, consumes enough current to ensure
 * the power bank will stay on.
 * To not waste too much power the on-time of this
 * circuit should be as short as possible but
 * long enough to keep the power bank alive in
 * all situations.
 *
 * To enable the module it is enough to define the
 * PIN_KEEP_ALIVE in the platformConfig.h file.
 * If disabled the calls are just empty and will
 * be thrown out by the optimizer.
 *
 * By defining PIN_KEEP_ALIVE_ON_STATE (HIGH or LOW),
 * PIN_KEEP_ALIVE_CICLE_MS and PIN_KEEP_ALIVE_ON_MS
 * the behavior can be adjusted as needed.
 */

#ifndef KEEPALIVE_HPP_
#define KEEPALIVE_HPP_

#include "platformConfig.h"
#include <Arduino.h>

namespace KeepAlive {

// define the defaults if not given by platformConfig.h
#ifndef PIN_KEEP_ALIVE_ON_STATE
#defined PIN_KEEP_ALIVE_ON_STATE  HIGH
#endif

#ifndef PIN_KEEP_ALIVE_CICLE_MS
#defined PIN_KEEP_ALIVE_CICLE_MS  10000  //ms
#endif

#ifndef PIN_KEEP_ALIVE_ON_MS
#defined PIN_KEEP_ALIVE_ON_MS  500  //ms
#endif

#if defined(PIN_KEEP_ALIVE)

static inline void set(bool on) {
	digitalWrite(PIN_KEEP_ALIVE, on ? PIN_KEEP_ALIVE_ON_STATE : !PIN_KEEP_ALIVE_ON_STATE);
}

static inline void loop() {
	systime_t ms = millis();
	// switch it on every PIN_KEEP_ALIVE_CICLE_MS ms for PIN_KEEP_ALIVE_CICLE_MS ms
	set( (ms % PIN_KEEP_ALIVE_CICLE_MS) < PIN_KEEP_ALIVE_ON_MS );
}

static inline void setup() {
	pinMode(PIN_KEEP_ALIVE, OUTPUT);
	// switch the circuit on during the startup phase
	set(true);
}

#else
static inline void set(bool) {};
static inline void loop() {};
static inline void setup() {};
#endif

} // END namespace KeepAlive

#endif /* KEEPALIVE_HPP_ */
