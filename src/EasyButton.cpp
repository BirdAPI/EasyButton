/*
 * @Author: Evert Arias 
 * @Date: 2019-08-18 17:34:49 
 * @Last Modified by: Evert Arias
 * @Last Modified time: 2019-08-18 20:22:10
 */

#include "EasyButton.h"

EasyButton::EasyButton(uint8_t pin, uint32_t debounce_time, bool pullup_enable, bool invert) : _pin(pin), _db_time(debounce_time), _invert(invert), _pu_enabled(pullup_enable)
{
	_read_method = POLL;
	pinMode(_pin, _pu_enabled ? INPUT_PULLUP : INPUT);
	_current_state = _readPin();
	if (_invert)
	{
		_current_state = !_current_state;
	}
	_time = millis();
	_last_state = _current_state;
	_changed = false;
	_last_change = _time;
}

bool EasyButton::read()
{
	uint32_t read_started_ms = millis();

	bool pinVal = _readPin();

	if (_invert)
	{
		pinVal = !pinVal;
	}

	if (read_started_ms - _last_change < _db_time)
	{
		// debounce time has not ellapsed
		_changed = false;
	}
	else
	{
		// debounce time ellapsed
		_last_state = _current_state;				// save last state
		_current_state = pinVal;					// assign new state as current state from pin's value
		_changed = (_current_state != _last_state); // report state change if current state vary from last state

		// if state has changed since last read
		if (_changed)
		{
			// save current millis as last change time
			_last_change = read_started_ms;
		}
	}

	if (wasReleased())
	{
		if (!_was_btn_held)
		{
			if (_short_press_count == 0)
			{
				_first_press_time = read_started_ms;
			}

			_short_press_count++;

			if (_pressed_callback)
			{
				_pressed_callback();
			}

			if (_short_press_count == _press_sequences && _press_sequence_duration >= (read_started_ms - _first_press_time))
			{
				// pressed_sequence
				if (_pressed_sequence_callback)
				{
					_pressed_sequence_callback();
				}
				_short_press_count = 0;
				_first_press_time = 0;
			}
			else if (_press_sequence_duration <= (read_started_ms - _first_press_time))
			{
				// sequence timeout
				_short_press_count = 0;
				_first_press_time = 0;
			}
		}
		// button was not held
		else
		{
			_was_btn_held = false;
		}
		// button released, reset _pressed_for_callbackCalled value
		_held_callback_called = false;
	}
	else if (isPressed() && _read_method == POLL)
	{
		_checkPressedTime();
	}

	_time = read_started_ms;

	return _current_state;
}

void EasyButton::update()
{
	if (!_was_btn_held)
	{
		_checkPressedTime();
	}
}

bool EasyButton::enableInterrupt()
{
	if (!_supportsInterrupt())
	{
		return false;
	}
	attachInterrupt(digitalPinToInterrupt(_pin), std::bind(&EasyButton::read, this), CHANGE);
	_read_method = INTERRUPT;
	return true;
}

void EasyButton::disableInterrupt()
{
	detachInterrupt(digitalPinToInterrupt(_pin));
	_read_method = POLL;
}

read_method_t EasyButton::setReadMethod(read_method_t read_method)
{
	if (read_method == INTERRUPT)
	{
		if (_supportsInterrupt())
		{
			attachInterrupt(digitalPinToInterrupt(_pin), std::bind(&EasyButton::read, this), CHANGE);
			_read_method = INTERRUPT;
			return _read_method;
		}
	}
	if (_read_method == INTERRUPT)
	{
		detachInterrupt(digitalPinToInterrupt(_pin));
	}
	_read_method = POLL;
	return _read_method;
}

read_method_t EasyButton::getReadMethod()
{
	return _read_method;
};

void EasyButton::onPressed(EasyButton::callback_t callback)
{
	_pressed_callback = callback;
}

void EasyButton::onPressedFor(uint32_t duration, EasyButton::callback_t callback)
{
	_held_threshold = duration;
	_pressed_for_callback = callback;
}

void EasyButton::onSequence(uint8_t sequences, uint32_t duration, EasyButton::callback_t callback)
{
	_press_sequences = sequences;
	_press_sequence_duration = duration;
	_pressed_sequence_callback = callback;
}

bool EasyButton::isPressed()
{
	return _current_state;
}

bool EasyButton::isReleased()
{
	return !_current_state;
}

bool EasyButton::wasPressed()
{
	return _current_state && _changed;
}

bool EasyButton::wasReleased()
{
	return !_current_state && _changed;
}

bool EasyButton::pressedFor(uint32_t duration)
{
	return _current_state && _time - _last_change >= duration;
}

bool EasyButton::releasedFor(uint32_t duration)
{
	return !_current_state && _time - _last_change >= duration;
}

/* Private */

bool EasyButton::_readPin()
{
	return digitalRead(_pin);
}

void EasyButton::_checkPressedTime()
{
	uint32_t read_started_ms = millis();
	if (_current_state && read_started_ms - _last_change >= _held_threshold && _pressed_for_callback)
	{
		// button has been pressed for at least the given time
		_was_btn_held = true;
		// reset short presses counters
		_short_press_count = 0;
		_first_press_time = 0;
		// call the callback function for a long press event if it exist and if it has not been called yet
		if (_pressed_for_callback && !_held_callback_called)
		{
			_held_callback_called = true; // set as called
			_pressed_for_callback();
		}
	}
}

bool EasyButton::_supportsInterrupt()
{
	return (digitalPinToInterrupt(_pin) != NOT_AN_INTERRUPT);
}