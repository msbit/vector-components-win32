#pragma once

#include "Framework.h"

namespace Util {
	void outputDebug(const char*, ...);
	bool outsideRect(const RECT*, long, long);
	float scale(float, float, float, float, float);

	template <class T> void SafeRelease(T** ptr)
	{
		if (*ptr)
		{
			(*ptr)->Release();
			*ptr = nullptr;
		}
	}
}