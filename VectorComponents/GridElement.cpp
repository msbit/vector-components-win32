#include <windowsx.h>

#include "Framework.h"
#include "GridElement.h"
#include "RenderTarget.h"
#include "Resource.h"
#include "Util.h"
#include "VectorComponentsElement.h"

GridElement::GridElement(HINSTANCE instance) {
	Register(instance, IDC_GRID, IDI_VECTORCOMPONENTS);
}

LRESULT GridElement::ProcessMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	RECT rect;
	RECT range = { -10, -10, 10, 10 };
	GetClientRect(window, &rect);

	switch (message)
	{
	case WM_CREATE:
	{
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory))) {
			return -1;
		}

		auto size = D2D1::SizeU(rect.right, rect.bottom);
		if (FAILED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(window, size), &renderTarget))) {
			Util::SafeRelease(&factory);
			return -1;
		}

		const auto color = D2D1::ColorF(1.0f, 1.0f, 0);
		if (FAILED(renderTarget->CreateSolidColorBrush(color, &brush))) {
			Util::SafeRelease(&renderTarget);
			Util::SafeRelease(&factory);
			return -1;
		}

		return 0;
	}
	case WM_DESTROY:
	{
		Util::SafeRelease(&brush);
		Util::SafeRelease(&renderTarget);
		Util::SafeRelease(&factory);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		if (Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			return 0;
		}

		getParent(window)->updateVectorFromMessage(&rect, lParam);

		mouseHeld = true;
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (!mouseHeld || Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			return 0;
		}

		getParent(window)->updateVectorFromMessage(&rect, lParam);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		if (Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			break;
		}

		getParent(window)->updateVectorFromMessage(&rect, lParam);

		mouseHeld = false;
		return 0;
	}
	case WM_PAINT:
	{
		if (renderTarget == nullptr) {
			return -1;
		}

		PAINTSTRUCT ps;
		if (nullptr == BeginPaint(window, &ps)) {
			return -1;
		}

		renderTarget->BeginDraw();

		renderTarget->Clear(D2D1::ColorF(GetSysColor(COLOR_WINDOW)));

		RenderTarget::drawGrid(renderTarget, &rect, &range);

		getParent(window)->drawVectors(renderTarget, &rect, &range);

		if (FAILED(renderTarget->EndDraw())) {
			return -1;
		}

		EndPaint(window, &ps);

		return 0;
	}
	case WM_SIZE:
	case WM_SIZING:
	{
		if (FAILED(renderTarget->Resize(D2D1::SizeU(rect.right, rect.bottom)))) {
			return -1;
		}

		return 0;
	}
	case WM_TIMER:
	{
		InvalidateRect(window, &rect, 1);
		return 0;
	}
	}
	return DefWindowProc(window, message, wParam, lParam);
}
