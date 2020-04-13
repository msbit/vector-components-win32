// WindowsProject1.cpp : Defines the entry point for the application.
//
#include <tuple>
#include <vector>

#include <WindowsX.h>

#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

namespace WindowsProject1 {
	enum selected_vector_t { none, a, b };

	HINSTANCE__ *instance;                                // current instance
	wchar_t title[MAX_LOADSTRING];                  // The title bar text
	wchar_t windowClass[MAX_LOADSTRING];            // the main window class name
	bool rotate = false;
	bool jitter = false;

	selected_vector_t selectedVector = none;
	std::tuple<float, float> vectorA;
	std::tuple<float, float> vectorB;

	unsigned short                registerWindowClass(HINSTANCE__ *);
	int                initInstance(HINSTANCE__ *, int);
	long __stdcall    loop(HWND__ *, unsigned int, unsigned int, long);
	void drawGrid(HDC__*, tagRECT *, int, int, int, int);
	void drawVectors(HDC__*, tagRECT *, int, int, int, int);
	float scale(float, float, float, float, float);
}

int __stdcall wWinMain(_In_ HINSTANCE__ *instance,
	_In_opt_ HINSTANCE__ *previousInstance,
	_In_ wchar_t    *cmdLine,
	_In_ int       cmdShow)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	LoadStringW(instance, IDS_APP_TITLE, WindowsProject1::title, MAX_LOADSTRING);
	LoadStringW(instance, IDC_WINDOWSPROJECT1, WindowsProject1::windowClass, MAX_LOADSTRING);
	WindowsProject1::registerWindowClass(instance);

	if (!WindowsProject1::initInstance(instance, cmdShow))
	{
		return 0;
	}

	tagMSG message;

	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return (int)message.wParam;
}

namespace WindowsProject1 {
	unsigned short registerWindowClass(HINSTANCE__ *instance)
	{
		tagWNDCLASSEXW wcex;

		wcex.cbSize = sizeof(tagWNDCLASSEXW);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = loop;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = instance;
		wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH__ *)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
		wcex.lpszClassName = windowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassExW(&wcex);
	}

	int initInstance(HINSTANCE__ *_instance, int cmdShow)
	{
		instance = _instance;

		const auto window = CreateWindowW(windowClass, title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, _instance, nullptr);

		if (!window)
		{
			return 0;
		}

		ShowWindow(window, cmdShow);
		UpdateWindow(window);

		SetTimer(window, 42, 1000 / 25, 0);

		return 1;
	}

	long __stdcall loop(HWND__ *window, unsigned int message, unsigned int wParam, long lParam)
	{
		tagRECT rect;
		GetClientRect(window, &rect);
		const auto padding = 10;
		const auto controlWidth = 90;
		const auto left = rect.right - (controlWidth + padding);

		switch (message)
		{
		case WM_CREATE:
		{
			const auto controlHeight = 35;
			HINSTANCE__ *instance = ((tagCREATESTRUCTW *)lParam)->hInstance;
			CreateWindow(L"button", L"A",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
				left, padding + (0 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_RADIO_A, instance, 0);
			CreateWindow(L"button", L"B",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				left, padding + (1 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_RADIO_B, instance, 0);
			CreateWindow(L"button", L"Rotate",
				WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				left, padding + (2 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_CHECK_ROTATE, instance, 0);
			CreateWindow(L"button", L"Jitter",
				WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				left, padding + (3 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_CHECK_JITTER, instance, 0);
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDC_RADIO_A:
				if (IsDlgButtonChecked(window, IDC_RADIO_A)) {
					selectedVector = a;
				}
				break;
			case IDC_RADIO_B:
				if (IsDlgButtonChecked(window, IDC_RADIO_B)) {
					selectedVector = b;
				}
				break;
			case IDC_CHECK_ROTATE:
				rotate = IsDlgButtonChecked(window, IDC_CHECK_ROTATE);
				break;
			case IDC_CHECK_JITTER:
				jitter = IsDlgButtonChecked(window, IDC_CHECK_JITTER);
				break;
			}

			return DefWindowProc(window, message, wParam, lParam);
			break;
		case WM_LBUTTONUP:
		{
			rect.left += padding;
			rect.right -= (controlWidth + padding);
			rect.top += padding;
			rect.bottom += padding;

			const auto canvasX = GET_X_LPARAM(lParam) + rect.left;
			const auto canvasY = GET_Y_LPARAM(lParam) + rect.top;

			const auto x = scale((float)rect.left, (float)rect.right, (float)-10, (float)10, (float)canvasX);
			const auto y = scale((float)rect.top, (float)rect.bottom, (float)-10, (float)10, (float)canvasY);

			if (selectedVector == a) {
				std::get<0>(vectorA) = x;
				std::get<1>(vectorA) = y;
			}

			if (selectedVector == b) {
				std::get<0>(vectorB) = x;
				std::get<1>(vectorB) = y;
			}
			break;
		}
		case WM_PAINT:
		{
			rect.left += padding;
			rect.right -= (controlWidth + padding);
			rect.top += padding;
			rect.bottom += padding;

			tagPAINTSTRUCT ps;
			const auto context = BeginPaint(window, &ps);

			FillRect(context, &ps.rcPaint, (HBRUSH__ *)(COLOR_WINDOW + 1));

			SelectObject(context, GetStockObject(DC_PEN));

			drawGrid(context, &rect, -10, 10, -10, 10);
			drawVectors(context, &rect, -10, 10, -10, 10);

			EndPaint(window, &ps);

			break;
		}
		case WM_TIMER:
			InvalidateRect(window, 0, 1);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(window, message, wParam, lParam);
		}
		return 0;
	}

	void drawGrid(HDC__ *context, tagRECT *rect, int minX, int maxX, int minY, int maxY) {
		for (auto x = minX; x <= maxX; x++) {
			const auto canvasX = (int)scale((float)minX, (float)maxX, (float)rect->left, (float)rect->right, (float)x);
			SetDCPenColor(context, x == 0 ? RGB(0x80, 0x80, 0x80) : RGB(0xD3, 0xD3, 0xD3));
			BeginPath(context);
			MoveToEx(context, canvasX, rect->top, 0);
			LineTo(context, canvasX, rect->bottom);
			EndPath(context);
			StrokePath(context);
		}

		for (auto y = minY; y <= maxY; y++) {
			const auto canvasY = (int)scale((float)minY, (float)maxY, (float)rect->top, (float)rect->bottom, (float)y);
			SetDCPenColor(context, y == 0 ? RGB(0x80, 0x80, 0x80) : RGB(0xD3, 0xD3, 0xD3));
			BeginPath(context);
			MoveToEx(context, rect->left, canvasY, 0);
			LineTo(context, rect->right, canvasY);
			EndPath(context);
			StrokePath(context);
		}
	}

	float scale(float inputMin, float inputMax, float outputMin, float outputMax, float input) {
		const auto inputRange = inputMax - inputMin;
		const auto outputRange = outputMax - outputMin;

		return ((input - inputMin) * (outputRange / inputRange)) + outputMin;
	}

	void drawVectors(HDC__ *context, tagRECT *rect, int minX, int maxX, int minY, int maxY) {
		if (std::get<0>(vectorA) != 0. && std::get<1>(vectorA) != 0.) {
			SetDCPenColor(context, RGB(0, 0, 0xFF));

			const auto x = std::get<0>(vectorA);
			const auto y = std::get<1>(vectorA);

			const auto canvasX = (int)scale((float)minX, (float)maxX, (float)rect->left, (float)rect->right, x);
			const auto canvasY = (int)scale((float)minY, (float)maxY, (float)rect->top, (float)rect->bottom, y);

			BeginPath(context);
			MoveToEx(context, (rect->left + rect->right) / 2, (rect->top + rect->bottom) / 2, 0);
			LineTo(context, canvasX, canvasY);
			EndPath(context);
			StrokePath(context);
		}
		
		if (std::get<0>(vectorB) != 0. && std::get<1>(vectorB) != 0.) {
			SetDCPenColor(context, RGB(0, 0xFF, 0));

			const auto x = std::get<0>(vectorB);
			const auto y = std::get<1>(vectorB);

			const auto canvasX = (int)scale((float)minX, (float)maxX, (float)rect->left, (float)rect->right, x);
			const auto canvasY = (int)scale((float)minY, (float)maxY, (float)rect->top, (float)rect->bottom, y);

			BeginPath(context);
			MoveToEx(context, (rect->left + rect->right) / 2, (rect->top + rect->bottom) / 2, 0);
			LineTo(context, canvasX, canvasY);
			EndPath(context);
			StrokePath(context);
		}
	}
}