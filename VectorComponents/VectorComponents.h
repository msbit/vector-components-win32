#pragma once

#include "resource.h"

namespace VectorComponents {
	void drawVectors(HDC, RECT*, RECT*);
	void updateVectorFromMessage(RECT*, LPARAM);
}