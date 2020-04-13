// WindowsProject1.cpp : Defines the entry point for the application.
//
#include <tuple>
#include <vector>

#include <WindowsX.h>

#include "framework.h"
#include "VectorComponents.h"
#include "context.h"
#include "util.h"
#include "vector.h"

#define MAX_LOADSTRING 100

namespace VectorComponents {
	enum selected_vector_t { NONE, VECTOR_A, VECTOR_B };

	HINSTANCE__ *instance;                                // current instance
	wchar_t title[MAX_LOADSTRING];                  // The title bar text
	wchar_t windowClass[MAX_LOADSTRING];            // the main window class name
	bool rotate = false;
	bool jitter = false;
	bool mouseHeld = false;

	selected_vector_t selectedVector = NONE;
	std::tuple<float, float> vectorA;
	std::tuple<float, float> vectorB;

	unsigned short                registerWindowClass(HINSTANCE__ *);
	int                initInstance(HINSTANCE__ *, int);
	long __stdcall    loop(HWND__ *, unsigned int, unsigned int, long);
	void drawVectors(HDC__ *, tagRECT *, tagRECT *);
	void update();
	void updateVectorFromMessage(tagRECT *, long);
}

int __stdcall wWinMain(_In_ HINSTANCE__ *instance,
	_In_opt_ HINSTANCE__ *previousInstance,
	_In_ wchar_t    *cmdLine,
	_In_ int       cmdShow)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	LoadStringW(instance, IDS_APP_TITLE, VectorComponents::title, MAX_LOADSTRING);
	LoadStringW(instance, IDC_VECTORCOMPONENTS, VectorComponents::windowClass, MAX_LOADSTRING);
	VectorComponents::registerWindowClass(instance);

	if (!VectorComponents::initInstance(instance, cmdShow))
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

namespace VectorComponents {
	unsigned short registerWindowClass(HINSTANCE__ *instance)
	{
		tagWNDCLASSEXW wcex;

		wcex.cbSize = sizeof(tagWNDCLASSEXW);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = loop;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = instance;
		wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_VECTORCOMPONENTS));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH__ *)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VECTORCOMPONENTS);
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

		SetTimer(window, 42, 1000 / 25, nullptr);

		return 1;
	}

	long __stdcall loop(HWND__ *window, unsigned int message, unsigned int wParam, long lParam)
	{
		tagRECT fullClientRect;
		tagRECT gridClientRect;
		tagRECT range = { -10, -10, 10, 10 };
		GetClientRect(window, &fullClientRect);
		GetClientRect(window, &gridClientRect);
		const auto padding = 10;
		const auto controlWidth = 90;
		const auto controlHeight = 35;
		const auto left = fullClientRect.right - (controlWidth + padding);

		gridClientRect.left += padding;
		gridClientRect.right -= (controlWidth + padding);
		gridClientRect.top += padding;
		gridClientRect.bottom += padding;

		switch (message)
		{
		case WM_CREATE:
		{
			HINSTANCE__ *instance = ((tagCREATESTRUCTW *)lParam)->hInstance;
			CreateWindow(L"button", L"A",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
				left, padding + (0 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_RADIO_A, instance, nullptr);
			CreateWindow(L"button", L"B",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				left, padding + (1 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_RADIO_B, instance, nullptr);
			CreateWindow(L"button", L"Rotate",
				WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				left, padding + (2 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_CHECK_ROTATE, instance, nullptr);
			CreateWindow(L"button", L"Jitter",
				WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				left, padding + (3 * controlHeight), controlWidth, controlHeight,
				window, (HMENU__ *)IDC_CHECK_JITTER, instance, nullptr);
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDC_RADIO_A:
				if (IsDlgButtonChecked(window, IDC_RADIO_A)) {
					selectedVector = VECTOR_A;
				}
				break;
			case IDC_RADIO_B:
				if (IsDlgButtonChecked(window, IDC_RADIO_B)) {
					selectedVector = VECTOR_B;
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
		case WM_LBUTTONDOWN:
			if (util::outsideRect(&gridClientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
				break;
			}

			updateVectorFromMessage(&gridClientRect, lParam);

			mouseHeld = true;
			break;
		case WM_MOUSEMOVE:
			if (!mouseHeld || util::outsideRect(&gridClientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
				break;
			}

			updateVectorFromMessage(&gridClientRect, lParam);
			break;
		case WM_LBUTTONUP:
		{
			if (util::outsideRect(&gridClientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) {
				break;
			}

			updateVectorFromMessage(&gridClientRect, lParam);

			mouseHeld = false;
			break;
		}
		case WM_PAINT:
		{
			tagPAINTSTRUCT ps;
			const auto context = BeginPaint(window, &ps);

			FillRect(context, &ps.rcPaint, (HBRUSH__ *)(COLOR_WINDOW + 1));

			SelectObject(context, GetStockObject(DC_PEN));

			context::drawGrid(context, &gridClientRect, &range);
			drawVectors(context, &gridClientRect, &range);

			EndPaint(window, &ps);

			break;
		}
		case WM_SIZING:
		{
			SetWindowPos(GetDlgItem(window, IDC_RADIO_A), HWND_TOP, left, padding + (0 * controlHeight), 0, 0, SWP_NOSIZE);
			SetWindowPos(GetDlgItem(window, IDC_RADIO_B), HWND_TOP, left, padding + (1 * controlHeight), 0, 0, SWP_NOSIZE);
			SetWindowPos(GetDlgItem(window, IDC_CHECK_ROTATE), HWND_TOP, left, padding + (2 * controlHeight), 0, 0, SWP_NOSIZE);
			SetWindowPos(GetDlgItem(window, IDC_CHECK_JITTER), HWND_TOP, left, padding + (3 * controlHeight), 0, 0, SWP_NOSIZE);
			break;
		}
		case WM_TIMER:
			update();

			InvalidateRect(window, &gridClientRect, 1);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(window, message, wParam, lParam);
		}
		return 0;
	}

	void drawVectors(HDC__ *context, tagRECT *rect, tagRECT *range) {
		auto vectorASet = false;
		auto vectorBSet = false;
		if (std::get<0>(vectorA) != 0. && std::get<1>(vectorA) != 0.) {
			vectorASet = true;
		}

		if (std::get<0>(vectorB) != 0. && std::get<1>(vectorB) != 0.) {
			vectorBSet = true;
		}

		if (vectorASet) {
			SetDCPenColor(context, RGB(0, 0, 0xFF));
			context::drawVector(context, rect, vectorA, range);
			SetDCPenColor(context, RGB(0, 0, 0));
			context::drawVector(context, rect, vector::normalise(vectorA), range);
			context::drawVector(context, rect, vector::perp(vectorA), range);
		}

		if (vectorBSet) {
			SetDCPenColor(context, RGB(0, 0xFF, 0));
			context::drawVector(context, rect, vectorB, range);
			SetDCPenColor(context, RGB(0, 0, 0));
			context::drawVector(context, rect, vector::normalise(vectorB), range);
			context::drawVector(context, rect, vector::perp(vectorB), range);
		}

		if (vectorASet && vectorBSet) {
			SetDCPenColor(context, RGB(0xFF, 0, 0));
			const auto perpB = vector::perp(vectorB);
			const auto normB = vector::normalise(vectorB);
			const auto aPerpB = vector::dotProduct(vectorA, perpB);
			const auto aNormB = vector::dotProduct(vectorA, normB);
			context::drawVector(context, rect, vector::scalarMultiple(perpB, aPerpB), range);
			context::drawVector(context, rect, vector::scalarMultiple(normB, aNormB), range);
		}
	}

	void update() {

		const auto magnitude = vector::magnitude(vectorA);
		if (magnitude > 0) {
			if (rotate) {
				auto angle = std::atan2f(std::get<1>(vectorA), std::get<0>(vectorA));
				angle += 0.01;
				std::get<0>(vectorA) = std::cosf(angle) * magnitude;
				std::get<1>(vectorA) = std::sinf(angle) * magnitude;
			}
			if (jitter) {
				std::get<0>(vectorA) += (((float)std::rand() / RAND_MAX) - 0.5) * 0.1;
				std::get<1>(vectorA) += (((float)std::rand() / RAND_MAX) - 0.5) * 0.1;
			}
		}


	}

	void updateVectorFromMessage(tagRECT *rect, long lParam) {
		const auto canvasX = GET_X_LPARAM(lParam) + rect->left;
		const auto canvasY = GET_Y_LPARAM(lParam) + rect->top;

		const auto x = util::scale((float)rect->left, (float)rect->right, (float)-10, (float)10, (float)canvasX);
		const auto y = util::scale((float)rect->top, (float)rect->bottom, (float)-10, (float)10, (float)canvasY);

		if (selectedVector == VECTOR_A) {
			std::get<0>(vectorA) = x;
			std::get<1>(vectorA) = y;
		}

		if (selectedVector == VECTOR_B) {
			std::get<0>(vectorB) = x;
			std::get<1>(vectorB) = y;
		}
	}
}