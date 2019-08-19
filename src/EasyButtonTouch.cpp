/*
 * @Author: Evert Arias, Gutierrez PS
 * @Date: 2019-08-18 19:58:49 
 * @Last Modified by: Evert Arias
 * @Last Modified time: 2019-08-18 19:59:36
 */

#if defined(ESP32)
#include "EasyButtonTouch.h"

void EasyButtonTouch::EasyButtonTouch()
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