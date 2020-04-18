#pragma once

#include <tuple>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "framework.h"

namespace renderTarget {
	void drawGrid(ID2D1HwndRenderTarget*, RECT*, RECT*);
	void drawVector(ID2D1HwndRenderTarget*, RECT*, std::tuple<float, float>, RECT*, D2D1::ColorF);
}