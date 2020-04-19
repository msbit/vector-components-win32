#pragma once

#include "Resource.h"

namespace VectorComponents {
	void drawVectors(ID2D1HwndRenderTarget*, RECT*, RECT*);
	void updateVectorFromMessage(RECT*, LPARAM);
}