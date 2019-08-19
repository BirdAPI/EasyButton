/*
 * @Author: Evert Arias, Gutierrez PS
 * @Date: 2019-08-18 19:58:49 
 * @Last Modified by: Evert Arias
 * @Last Modified time: 2019-08-18 22:12:26
 */

#if defined(ESP32)
#include "EasyButtonTouch.h"

EasyButtonTouch::EasyButtonTouch(uint8_t pin, uint32_t debounce_time, uint16_t threshold) : EasyButton(pin, debounce_time, false, false), _touch_threshold(threshold)
{
	_current_state = _readPin();
	_time = millis();
	_last_state = _current_state;
	_changed = false;
	_last_change = _time;
}

bool EasyButtonTouch::_readPin()
{
	return touchRead(_pin) < _touch_threshold;
}

#endif