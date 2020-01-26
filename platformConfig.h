/*
 * hwConfig.h
 *
 *  Created on: 30.11.2019
 *      Author: nick
 */

#ifndef PLATFORMCONFIG_H_
#define PLATFORMCONFIG_H_

typedef unsigned long systime_t;

// Global settings
#define LONG_PRESS       1000 // milliseconds
#define DEFAULT_VOLUME     10 // default value for volume setings (0-30)

// uncomment the below line to enable remix of queue at end
#define RECREATE_QUEUE_ON_END

// Global power management pins
#define PIN_OPEN_ANALOG    A7

#define PIN_BUTTON_LED_PWM	           3
#define PIN_BUTTON_LED_PWM_DEFAULT    50

#define PIN_KEEP_ALIVE                 5
// actually these are the same as the module defaults
#define PIN_KEEP_ALIVE_ON_STATE     HIGH
#define PIN_KEEP_ALIVE_CICLE_MS    10000
#define PIN_KEEP_ALIVE_ON_MS         500

#define PIN_POWER_SWITCH               7
#define PIN_POWER_SWITCH_ON_STATE   HIGH

// Pins used for communication with the DF-Player module
// RX on the Arduino goes to DF-Player TX and vice versa.
#define PIN_DFPLAYER_RX         2
#define PIN_DFPLAYER_TX         8
#define PIN_DFPLAYER_BUSY       4

// MFRC522 - RFID card reader
#define PIN_RFID_READER_RST      9
#define PIN_RFID_READER_SS      10

// Light sensor pin
#define LIGHT_SENSE        A0

// Button pins
#define PIN_BUTTON_PAUSE       A5
#define PIN_BUTTON_DOWN        A4
#define PIN_BUTTON_UP          A3
#define PIN_BUTTON_VOL_DOWN    A2
#define PIN_BUTTON_VOL_UP      A1

#endif /* PLATFORMCONFIG_H_ */
