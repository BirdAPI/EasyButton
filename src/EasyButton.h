/*
 * @Author: Evert Arias 
 * @Date: 2019-08-18 17:34:40 
 * @Last Modified by: Evert Arias
 * @Last Modified time: 2019-08-18 20:21:59
 */

#ifndef _EasyButton_h
#define _EasyButton_h

#include <Arduino.h>

#ifdef ESP8266
#define EASYBUTTON_FUNCTIONAL_SUPPORT 1
#endif

#ifdef EASYBUTTON_FUNCTIONAL_SUPPORT
#include <functional>
#include "FunctionalInterrupt.h"
#endif

enum read_method_t
{
	INTERRUPT,
	POLL
};

class EasyButton
{
	friend class EasyButtonTouch;

public:
#ifdef EASYBUTTON_FUNCTIONAL_SUPPORT
	typedef std::function<void()> callback_t;
#else
	typedef void (*callback_t)();
#endif

	EasyButton(uint8_t pin, uint32_t debounce_time = 35, bool pullup_enable = true, bool invert = true);

	~EasyButton() {}

	bool read(read_method_t read_method = POLL); // Returns the current debounced button state, true for pressed, false for released
	void update();								 // Update button pressed time, only needed when using interrupts

	bool enableInterrupt(); // Call a callback function when the button is pressed or released
	void disableInterrupt();

	void onPressed(callback_t callback);										// Call a callback function when the button has been pressed and released
	void onPressedFor(uint32_t duration, callback_t callback);					// Call a callback function when the button has been held for at least the given number of milliseconds
	void onSequence(uint8_t sequences, uint32_t duration, callback_t callback); // Call a callback function when the given sequence has matched

	bool isPressed();					 // Returns true if the button's state was pressed at the last read
	bool isReleased();					 // Returns true if the button's state was released at the last read
	bool wasPressed();					 // Returns true if the button's state at the last read was pressed
	bool wasReleased();					 // Returns true if the button's state at the last read was released
	bool pressedFor(uint32_t duration);  // Returns true if the button's state at the last read was pressed, and has been in that state for at least the given number of milliseconds
	bool releasedFor(uint32_t duration); // Returns true if the button's state at the last read was released, and has been in that state for at least the given number of milliseconds

private:
	uint32_t _short_press_count;	   // Short press counter
	uint32_t _first_press_time;		   // Time when button was pressed for first time
	uint8_t _press_sequences;		   // The number of sequences to count
	uint32_t _press_sequence_duration; // Time limit of the sequence
	uint32_t _held_threshold;		   // Held threshold
	bool _was_btn_held;				   // Indicate if button was held
	bool _held_callback_called;		   // Indicate if button long press has been notified
	uint8_t _pin;					   // Arduino pin number where the Button is connected to
	uint32_t _db_time;				   // Debounce time (ms)
	bool _invert;					   // Inverts button logic. If true, low = pressed else high = pressed
	bool _pu_enabled;				   // Internal pullup resistor enabled
	bool _current_state;			   // Current button state, true = pressed
	bool _last_state;				   // Previous button state, true = pressed
	bool _changed;					   // Has the state change since last read
	uint32_t _time;					   // Time of current state
	uint32_t _last_change;			   // Time of last state change

	callback_t _pressed_callback;		   // Callback function for pressed events
	callback_t _pressed_for_callback;	  // Callback function for pressedFor events
	callback_t _pressed_sequence_callback; // Callback function for pressedSequence events

	virtual bool _readPin();   // Abstracts the pin value reading
	void _checkPressedTime();  // Verify if pressed_for_callback should be called
	bool _supportsInterrupt(); // Returns true if the button's pin is an external interrupt pin
};

#endif