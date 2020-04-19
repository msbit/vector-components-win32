#pragma once

#include <tuple>

#include "Framework.h"

namespace Context {
	void drawGrid(HDC, RECT*, RECT*);
	void drawLine(HDC, POINT, POINT);
	void drawVector(HDC, RECT*, std::tuple<float, float>, RECT*);
}