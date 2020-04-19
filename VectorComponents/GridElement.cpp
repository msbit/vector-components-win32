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
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
		auto size = D2D1::SizeU(rect.right, rect.bottom);
		factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(window, size),
			&renderTarget);
		const auto color = D2D1::ColorF(1.0f, 1.0f, 0);
		renderTarget->CreateSolidColorBrush(color, &brush);
		break;
	}
	case WM_DESTROY:
	{
		brush->Release();
		renderTarget->Release();
		factory->Release();
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			break;
		}

		getParent(window)->updateVectorFromMessage(&rect, lParam);

		mouseHeld = true;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (!mouseHeld || Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			break;
		}

		getParent(window)->updateVectorFromMessage(&rect, lParam);
		break;
	}
	case WM_LBUTTONUP:
	{
		if (Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			break;
		}

		getParent(window)->updateVectorFromMessage(&rect, lParam);

		mouseHeld = false;
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		const auto context = BeginPaint(window, &ps);

		renderTarget->BeginDraw();

		renderTarget->Clear(D2D1::ColorF(GetSysColor(COLOR_WINDOW)));

		RenderTarget::drawGrid(renderTarget, &rect, &range);

		getParent(window)->drawVectors(renderTarget, &rect, &range);

		renderTarget->EndDraw();

		EndPaint(window, &ps);

		break;
	}
	case WM_SIZE:
	case WM_SIZING:
	{
		auto size = D2D1::SizeU(rect.right, rect.bottom);

		renderTarget->Resize(size);
		break;
	}
	case WM_TIMER:
	{
		InvalidateRect(window, &rect, 1);
		break;
	}
	default:
	{
		return DefWindowProc(window, message, wParam, lParam);
	}
	}
	return 0;
}
