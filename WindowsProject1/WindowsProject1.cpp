// WindowsProject1.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

namespace WindowsProject1 {
	// Global Variables:
	HINSTANCE__ *instance;                                // current instance
	wchar_t title[MAX_LOADSTRING];                  // The title bar text
	wchar_t windowClass[MAX_LOADSTRING];            // the main window class name

	// Forward declarations of functions included in this code module:
	unsigned short                RegisterWindowClass(HINSTANCE__ *);
	int                InitInstance(HINSTANCE__ *, int);
	long __stdcall    Loop(HWND__ *, unsigned int, unsigned int, long);
}

int __stdcall wWinMain(_In_ HINSTANCE__ *instance,
	_In_opt_ HINSTANCE__ *previousInstance,
	_In_ wchar_t    *cmdLine,
	_In_ int       cmdShow)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(instance, IDS_APP_TITLE, WindowsProject1::title, MAX_LOADSTRING);
	LoadStringW(instance, IDC_WINDOWSPROJECT1, WindowsProject1::windowClass, MAX_LOADSTRING);
	WindowsProject1::RegisterWindowClass(instance);

	// Perform application initialization:
	if (!WindowsProject1::InitInstance(instance, cmdShow))
	{
		return 0;
	}

	tagMSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

namespace WindowsProject1 {

	//
	//  FUNCTION: RegisterWindowClass()
	//
	//  PURPOSE: Registers the window class.
	//
	unsigned short RegisterWindowClass(HINSTANCE__ *instance)
	{
		tagWNDCLASSEXW wcex;

		wcex.cbSize = sizeof(tagWNDCLASSEXW);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Loop;
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

	//
	//   FUNCTION: InitInstance(HINSTANCE, int)
	//
	//   PURPOSE: Saves instance handle and creates main window
	//
	//   COMMENTS:
	//
	//        In this function, we save the instance handle in a global variable and
	//        create and display the main program window.
	//
	int InitInstance(HINSTANCE__ *_instance, int cmdShow)
	{
		instance = _instance; // Store instance handle in our global variable

		HWND__ *window = CreateWindowW(windowClass, title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, _instance, nullptr);

		if (!window)
		{
			return 0;
		}

		ShowWindow(window, cmdShow);
		UpdateWindow(window);

		return 1;
	}

	//
	//  FUNCTION: Loop(HWND, UINT, WPARAM, LPARAM)
	//
	//  PURPOSE: Processes messages for the main window.
	//
	//  WM_COMMAND  - process the application menu
	//  WM_PAINT    - Paint the main window
	//  WM_DESTROY  - post a quit message and return
	//
	//
	long __stdcall Loop(HWND__ *window, unsigned int message, unsigned int wParam, long lParam)
	{
		switch (message)
		{
		case WM_COMMAND:
		{
			return DefWindowProc(window, message, wParam, lParam);
		}
		break;
		case WM_PAINT:
		{
			tagPAINTSTRUCT ps;
			HDC__ *context = BeginPaint(window, &ps);

			FillRect(context, &ps.rcPaint, (HBRUSH__ *)(COLOR_WINDOW + 1));

			EndPaint(window, &ps);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(window, message, wParam, lParam);
		}
		return 0;
	}
}