#pragma once

#include "framework.h"

namespace context {
	void drawGrid(HDC__ *, tagRECT *, tagRECT *);
	void drawVector(HDC__ *, tagRECT *, std::tuple<float, float>, tagRECT *);
}