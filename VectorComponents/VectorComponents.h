#pragma once

#include "resource.h"

namespace VectorComponents {
	void drawVectors(ID2D1HwndRenderTarget*, RECT*, RECT*);
	void updateVectorFromMessage(RECT*, LPARAM);
}