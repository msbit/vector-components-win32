#define _USE_MATH_DEFINES
#include <cmath>

#include <tuple>

#include "framework.h"

#include "context.h"
#include "util.h"

namespace context {
	void drawGrid(HDC context, RECT* rect, RECT* range) {
		for (auto x = range->left; x <= range->right; x++) {
			const auto canvasX = (int)util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, (float)x);
			SetDCPenColor(context, x == 0 ? RGB(0x80, 0x80, 0x80) : RGB(0xD3, 0xD3, 0xD3));

			drawLine(context, { canvasX, rect->top }, { canvasX, rect->bottom });
		}

		for (auto y = range->top; y <= range->bottom; y++) {
			const auto canvasY = (int)util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, (float)y);
			SetDCPenColor(context, y == 0 ? RGB(0x80, 0x80, 0x80) : RGB(0xD3, 0xD3, 0xD3));

			drawLine(context, { rect->left, canvasY }, { rect->right, canvasY });
		}
	}

	void drawLine(HDC context, POINT start, POINT end) {
		BeginPath(context);
		MoveToEx(context, start.x, start.y, nullptr);
		LineTo(context, end.x, end.y);
		EndPath(context);
		StrokePath(context);
	}

	void drawVector(HDC context, RECT* rect, std::tuple<float, float> vector, RECT* range) {
		const auto x = std::get<0>(vector);
		const auto y = std::get<1>(vector);

		const auto canvasX = (int)util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x);
		const auto canvasY = (int)util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y);

		drawLine(context, { (rect->left + rect->right) / 2,  (rect->top + rect->bottom) / 2 }, { canvasX, canvasY });

		const auto angle = atan2f(y, x) + (float)M_PI;
		const auto ccw = angle + 0.2f;
		const auto cw = angle - 0.2f;
		const auto ccwX = (int)util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x + 0.5f * cosf(ccw));
		const auto ccwY = (int)util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y + 0.5f * sinf(ccw));
		const auto cwX = (int)util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x + 0.5f * cosf(cw));
		const auto cwY = (int)util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y + 0.5f * sinf(cw));
		const POINT points[3] = {
			{ canvasX, canvasY },
			{ ccwX, ccwY },
			{ cwX, cwY }
		};

		Polygon(context, points, 3);
	}
}