#define _USE_MATH_DEFINES
#include <cmath>
#include <tuple>

#include "Framework.h"
#include "RenderTarget.h"
#include "Util.h"

namespace RenderTarget {
	void drawGrid(ID2D1HwndRenderTarget* renderTarget, RECT* rect, RECT* range) {
		ID2D1SolidColorBrush* majorBrush;
		ID2D1SolidColorBrush* minorBrush;

		if (FAILED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.5f), &majorBrush))) {
			return;
		}

		if (FAILED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.82421875f, 0.82421875f, 0.82421875f), &minorBrush))) {
			Util::SafeRelease(&majorBrush);
			return;
		}

		for (auto x = range->left; x <= range->right; x++) {
			const auto canvasX = Util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, (float)x);
			renderTarget->DrawLine({ canvasX, (float)rect->top }, { canvasX, (float)rect->bottom }, x == 0 ? majorBrush : minorBrush);
		}

		for (auto y = range->top; y <= range->bottom; y++) {
			const auto canvasY = Util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, (float)y);
			renderTarget->DrawLine({ (float)rect->left, canvasY }, { (float)rect->right, canvasY }, y == 0 ? majorBrush : minorBrush);
		}

		Util::SafeRelease(&minorBrush);
		Util::SafeRelease(&majorBrush);
	}

	void drawVector(ID2D1HwndRenderTarget* renderTarget, RECT* rect, std::tuple<float, float> vector, RECT* range, D2D1::ColorF colour) {
		ID2D1SolidColorBrush* brush;
		if (FAILED(renderTarget->CreateSolidColorBrush(colour, &brush))) {
			return;
		}

		const auto x = std::get<0>(vector);
		const auto y = std::get<1>(vector);

		const auto canvasX = Util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x);
		const auto canvasY = Util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y);

		renderTarget->DrawLine({ (float)(rect->left + rect->right) / 2,  (float)(rect->top + rect->bottom) / 2 }, { canvasX, canvasY }, brush);

		ID2D1Factory* factory;
		renderTarget->GetFactory(&factory);
		ID2D1PathGeometry* geometry;
		if (FAILED(factory->CreatePathGeometry(&geometry))) {
			Util::SafeRelease(&factory);
			Util::SafeRelease(&brush);
			return;
		}

		ID2D1GeometrySink* sink;
		if (FAILED(geometry->Open(&sink))) {
			Util::SafeRelease(&geometry);
			Util::SafeRelease(&factory);
			Util::SafeRelease(&brush);
			return;
		}

		sink->BeginFigure({ canvasX, canvasY }, D2D1_FIGURE_BEGIN_FILLED);

		const auto angle = atan2f(y, x) + (float)M_PI;
		const auto ccw = angle + 0.2f;
		const auto ccwX = Util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x + 0.5f * cosf(ccw));
		const auto ccwY = Util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y + 0.5f * sinf(ccw));
		sink->AddLine({ ccwX, ccwY });

		const auto cw = angle - 0.2f;
		const auto cwX = Util::scale((float)range->left, (float)range->right, (float)rect->left, (float)rect->right, x + 0.5f * cosf(cw));
		const auto cwY = Util::scale((float)range->top, (float)range->bottom, (float)rect->top, (float)rect->bottom, y + 0.5f * sinf(cw));
		sink->AddLine({ cwX, cwY });

		sink->EndFigure(D2D1_FIGURE_END_CLOSED);

		if (FAILED(sink->Close())) {
			Util::SafeRelease(&sink);
			Util::SafeRelease(&geometry);
			Util::SafeRelease(&factory);
			Util::SafeRelease(&brush);
			return;
		}

		renderTarget->FillGeometry(geometry, brush);

		Util::SafeRelease(&sink);
		Util::SafeRelease(&geometry);
		Util::SafeRelease(&factory);
		Util::SafeRelease(&brush);
	}
}