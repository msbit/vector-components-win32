// WindowsProject1.cpp : Defines the entry point for the application.
//
#include <tuple>
#include <vector>

#include <WindowsX.h>

#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

namespace WindowsProject1 {
	HINSTANCE__ *instance;                                // current instance
	wchar_t title[MAX_LOADSTRING];                  // The title bar text
	wchar_t windowClass[MAX_LOADSTRING];            // the main window class name
	std::vector<std::tuple<float, float>> vectors;

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
		switch (message)
		{
		case WM_COMMAND:
			return DefWindowProc(window, message, wParam, lParam);
			break;
		case WM_LBUTTONUP:
		{
			tagRECT rect;

			GetClientRect(window, &rect);

			const auto canvasX = GET_X_LPARAM(lParam) + rect.left;
			const auto canvasY = GET_Y_LPARAM(lParam) + rect.top;

			const auto x = scale((float)rect.left, (float)rect.right, (float)-10, (float)10, (float)canvasX);
			const auto y = scale((float)rect.top, (float)rect.bottom, (float)-10, (float)10, (float)canvasY);

			vectors.push_back(std::tuple<float, float>(x, y));
			break;
		}
		case WM_PAINT:
		{
			tagPAINTSTRUCT ps;
			const auto context = BeginPaint(window, &ps);

			FillRect(context, &ps.rcPaint, (HBRUSH__ *)(COLOR_WINDOW + 1));
			drawGrid(context, &ps.rcPaint, -10, 10, -10, 10);
			drawVectors(context, &ps.rcPaint, -10, 10, -10, 10);

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
			const auto canvasX = (int)scale((float)minX, (float)maxX, (float)0, (float)(rect->right - rect->left), (float)x);
			BeginPath(context);
			MoveToEx(context, canvasX, rect->top, 0);
			LineTo(context, canvasX, rect->bottom);
			EndPath(context);
			StrokePath(context);
		}

		for (auto y = minY; y <= maxY; y++) {
			const auto canvasY = (int)scale((float)minY, (float)maxY, (float)0, (float)(rect->bottom - rect->top), (float)y);
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
		SelectObject(context, GetStockObject(DC_PEN));
		SetDCPenColor(context, RGB(255, 0, 0));
		for (auto v : vectors) {
			const auto x = std::get<0>(v);
			const auto y = std::get<1>(v);

			const auto canvasX = (int)scale((float)minX, (float)maxX, 0, (float)(rect->right - rect->left), x);
			const auto canvasY = (int)scale((float)minY, (float)maxY, 0, (float)(rect->bottom - rect->top), y);

			BeginPath(context);
			MoveToEx(context, (rect->left + rect->right) / 2, (rect->top + rect->bottom) / 2, 0);
			LineTo(context, canvasX, canvasY);
			EndPath(context);
			StrokePath(context);
		}
	}
}