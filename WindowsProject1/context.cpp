#include <tuple>

#include "framework.h"

#include "util.h"

namespace context {
	void drawGrid(HDC__ *context, tagRECT *rect, tagRECT *range) {
		for (auto x = range->left; x <= range->right; x++) {
			const auto canvasX = (int)util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, (float)x);
			SetDCPenColor(context, x == 0 ? RGB(0x80, 0x80, 0x80) : RGB(0xD3, 0xD3, 0xD3));
			BeginPath(context);
			MoveToEx(context, canvasX, rect->top, 0);
			LineTo(context, canvasX, rect->bottom);
			EndPath(context);
			StrokePath(context);
		}

		for (auto y = range->top; y <= range->bottom; y++) {
			const auto canvasY = (int)util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, (float)y);
			SetDCPenColor(context, y == 0 ? RGB(0x80, 0x80, 0x80) : RGB(0xD3, 0xD3, 0xD3));
			BeginPath(context);
			MoveToEx(context, rect->left, canvasY, 0);
			LineTo(context, rect->right, canvasY);
			EndPath(context);
			StrokePath(context);
		}
	}
	void drawVector(HDC__ *context, tagRECT *rect, std::tuple<float, float> vector, tagRECT *range) {
		const auto x = std::get<0>(vector);
		const auto y = std::get<1>(vector);

		const auto canvasX = (int)util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x);
		const auto canvasY = (int)util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y);

		BeginPath(context);
		MoveToEx(context, (rect->left + rect->right) / 2, (rect->top + rect->bottom) / 2, 0);
		LineTo(context, canvasX, canvasY);
		EndPath(context);
		StrokePath(context);
	}
}