#include <windowsx.h>

#include "framework.h"
#include "grid.h"
#include "renderTarget.h"
#include "resource.h"
#include "util.h"
#include "VectorComponents.h"

bool VectorComponents::grid::mouseHeld;

namespace VectorComponents::grid {
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* renderTarget;
	ID2D1SolidColorBrush* brush;

	LRESULT CALLBACK loop(HWND, UINT, WPARAM, LPARAM);

	ATOM registerWindowClass(HINSTANCE instance)
	{
		WCHAR windowClass[MAX_LOADSTRING];

		LoadString(instance, IDC_GRID, windowClass, MAX_LOADSTRING);

		WNDCLASS wc = {
			.style = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc = loop,
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

	LRESULT CALLBACK loop(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
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
			if (util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
				break;
			}

			VectorComponents::updateVectorFromMessage(&rect, lParam);

			mouseHeld = true;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (!mouseHeld || util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
				break;
			}

			VectorComponents::updateVectorFromMessage(&rect, lParam);
			break;
		}
		case WM_LBUTTONUP:
		{
			if (util::outsideRect(&rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
				break;
			}

			VectorComponents::updateVectorFromMessage(&rect, lParam);

			mouseHeld = false;
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const auto context = BeginPaint(window, &ps);

			renderTarget->BeginDraw();

			renderTarget->Clear(D2D1::ColorF(GetSysColor(COLOR_WINDOW)));

			renderTarget::drawGrid(renderTarget, &rect, &range);
			VectorComponents::drawVectors(renderTarget, &rect, &range);

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
}