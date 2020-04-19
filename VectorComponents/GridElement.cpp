#include <windowsx.h>

#include "Framework.h"
#include "GridElement.h"
#include "RenderTarget.h"
#include "Resource.h"
#include "Util.h"
#include "VectorComponentsElement.h"

ATOM GridElement::RegisterWindowClass(HINSTANCE instance)
{
	WCHAR windowClass[MAX_LOADSTRING];

	LoadString(instance, IDC_GRID, windowClass, MAX_LOADSTRING);

	WNDCLASS wc = {
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = Loop,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = instance,
		.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_VECTORCOMPONENTS)),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = MAKEINTRESOURCEW(IDC_GRID),
		.lpszClassName = windowClass
	};

	return RegisterClass(&wc);
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

		HWND parentWindow = (HWND)GetWindowLongPtr(window, GWLP_HWNDPARENT);
		VectorComponentsElement* parent = (VectorComponentsElement*)GetWindowLongPtr(parentWindow, GWLP_USERDATA);

		parent->updateVectorFromMessage(&rect, lParam);

		mouseHeld = true;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (!mouseHeld || Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			break;
		}

		HWND parentWindow = (HWND)GetWindowLongPtr(window, GWLP_HWNDPARENT);
		VectorComponentsElement* parent = (VectorComponentsElement*)GetWindowLongPtr(parentWindow, GWLP_USERDATA);

		parent->updateVectorFromMessage(&rect, lParam);
		break;
	}
	case WM_LBUTTONUP:
	{
		if (Util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
			break;
		}

		HWND parentWindow = (HWND)GetWindowLongPtr(window, GWLP_HWNDPARENT);
		VectorComponentsElement* parent = (VectorComponentsElement*)GetWindowLongPtr(parentWindow, GWLP_USERDATA);

		parent->updateVectorFromMessage(&rect, lParam);

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

		HWND parentWindow = (HWND)GetWindowLongPtr(window, GWLP_HWNDPARENT);
		VectorComponentsElement* parent = (VectorComponentsElement*)GetWindowLongPtr(parentWindow, GWLP_USERDATA);

		parent->drawVectors(renderTarget, &rect, &range);

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
