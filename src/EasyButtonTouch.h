/*
 * @Author: Evert Arias, Gutierrez PS
 * @Date: 2019-08-18 19:58:49 
 * @Last Modified by: Evert Arias
 * @Last Modified time: 2019-08-18 19:59:13
 */

#if !defined(_EasyButtonTouch_h) and defined(ESP32)
#define _EasyButtonTouch_h

#include <Arduino.h>
#include "EasyButton.h"

class EasyButtonTouch : public EasyButton
{
public:
	EasyButtonTouch(uint8_t pin, uint32_t debounce_time = 35, uint16_t threshold = 50) : EasyButton(pin, debounce_time, false, false), _touch_threshold(threshold) {}

private:
	// If touchRead() is below the threshold, the button is considered pressed
	uint16_t _touch_threshold;
	bool _readPin();
};

#endif