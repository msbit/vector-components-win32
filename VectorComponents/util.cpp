#include <stdio.h>

#include "framework.h"

namespace util {
	bool outsideRect(const RECT* rect, long x, long y) {
		if (x < rect->left) {
			return true;
		}

		if (x > rect->right) {
			return true;
		}

		if (y < rect->top) {
			return true;
		}

		if (y > rect->bottom) {
			return true;
		}

		return false;
	}

	void outputDebug(const char* format, ...) {
		va_list argptr;
		va_start(argptr, format);
		char buffer[256];
		vsprintf_s(buffer, 256, format, argptr);
		OutputDebugStringA(buffer);
	}

	float scale(float inputMin, float inputMax, float outputMin, float outputMax, float input) {
		const auto inputRange = inputMax - inputMin;
		const auto outputRange = outputMax - outputMin;

		return ((input - inputMin) * (outputRange / inputRange)) + outputMin;
	}
}