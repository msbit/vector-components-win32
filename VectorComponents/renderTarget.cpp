#define _USE_MATH_DEFINES
#include <cmath>
#include <tuple>

#include "framework.h"
#include "renderTarget.h"
#include "util.h"

namespace renderTarget {
	void drawGrid(ID2D1HwndRenderTarget* renderTarget, RECT* rect, RECT* range) {
		ID2D1SolidColorBrush* majorBrush;
		ID2D1SolidColorBrush* minorBrush;

		renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.5f), &majorBrush);
		renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.82421875f, 0.82421875f, 0.82421875f), &minorBrush);
		for (auto x = range->left; x <= range->right; x++) {
			const auto canvasX = util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, (float)x);
			renderTarget->DrawLine({ canvasX, (float)rect->top }, { canvasX, (float)rect->bottom }, x == 0 ? majorBrush : minorBrush);
		}

		for (auto y = range->top; y <= range->bottom; y++) {
			const auto canvasY = util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, (float)y);
			renderTarget->DrawLine({ (float)rect->left, canvasY }, { (float)rect->right, canvasY }, y == 0 ? majorBrush : minorBrush);
		}

		minorBrush->Release();
		majorBrush->Release();
	}

	void drawVector(ID2D1HwndRenderTarget* renderTarget, RECT* rect, std::tuple<float, float> vector, RECT* range, D2D1::ColorF colour) {
		ID2D1SolidColorBrush* brush;
		renderTarget->CreateSolidColorBrush(colour, &brush);

		const auto x = std::get<0>(vector);
		const auto y = std::get<1>(vector);

		const auto canvasX = util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x);
		const auto canvasY = util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y);

		renderTarget->DrawLine({ (float)(rect->left + rect->right) / 2,  (float)(rect->top + rect->bottom) / 2 }, { canvasX, canvasY }, brush);

		ID2D1Factory* factory;
		renderTarget->GetFactory(&factory);
		ID2D1PathGeometry* geometry;
		factory->CreatePathGeometry(&geometry);
		ID2D1GeometrySink* sink;
		geometry->Open(&sink);
		sink->BeginFigure({ canvasX, canvasY }, D2D1_FIGURE_BEGIN_FILLED);

		const auto angle = atan2f(y, x) + (float)M_PI;
		const auto ccw = angle + 0.2f;
		const auto ccwX = util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x + 0.5f * cosf(ccw));
		const auto ccwY = util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y + 0.5f * sinf(ccw));
		sink->AddLine({ ccwX, ccwY });

		const auto cw = angle - 0.2f;
		const auto cwX = util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x + 0.5f * cosf(cw));
		const auto cwY = util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y + 0.5f * sinf(cw));
		sink->AddLine({ cwX, cwY });

		sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		sink->Close();

		renderTarget->FillGeometry(geometry, brush);

		factory->Release();
		sink->Release();
		geometry->Release();
		brush->Release();
	}
}