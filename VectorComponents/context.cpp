#include <tuple>

#include "framework.h"

#include "context.h"
#include "util.h"

namespace context {
	void drawGrid(HDC context, RECT *rect, RECT *range) {
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

	void drawVector(HDC context, RECT *rect, std::tuple<float, float> vector, RECT *range) {
		const auto x = std::get<0>(vector);
		const auto y = std::get<1>(vector);

		const auto canvasX = (int)util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x);
		const auto canvasY = (int)util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y);

		drawLine(context, { (rect->left + rect->right) / 2,  (rect->top + rect->bottom) / 2 }, { canvasX, canvasY });
	}
}