#pragma once

#include <tuple>

#include "Framework.h"

namespace RenderTarget {
	void drawGrid(ID2D1HwndRenderTarget*, RECT*, RECT*);
	void drawVector(ID2D1HwndRenderTarget*, RECT*, std::tuple<float, float>, RECT*, D2D1::ColorF);
}