#pragma once

#include "Framework.h"

namespace util {
	void outputDebug(const char*, ...);
	bool outsideRect(const RECT*, long, long);
	float scale(float, float, float, float, float);
}