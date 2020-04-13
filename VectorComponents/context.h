#pragma once

#include "framework.h"

namespace context {
	void drawGrid(HDC, RECT *, RECT *);
	void drawVector(HDC, RECT *, std::tuple<float, float>, RECT *);
}