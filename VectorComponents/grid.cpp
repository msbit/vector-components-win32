#include <windowsx.h>

#include "framework.h"

#include "context.h"
#include "grid.h"
#include "resource.h"
#include "util.h"
#include "VectorComponents.h"

bool VectorComponents::grid::mouseHeld;

namespace VectorComponents::grid {
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

			FillRect(context, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			SelectObject(context, GetStockObject(DC_BRUSH));
			SelectObject(context, GetStockObject(DC_PEN));

			context::drawGrid(context, &rect, &range);
			VectorComponents::drawVectors(context, &rect, &range);

			EndPaint(window, &ps);

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