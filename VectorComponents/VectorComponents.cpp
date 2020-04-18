// WindowsProject1.cpp : Defines the entry point for the application.
//
#include <tuple>
#include <vector>

#include <windowsx.h>

#include "grid.h"
#include "framework.h"
#include "VectorComponents.h"
#include "context.h"
#include "util.h"
#include "vector.h"

#define MAX_LOADSTRING 100

namespace VectorComponents {
	enum class selected_vector_t { NONE, VECTOR_A, VECTOR_B };

	HINSTANCE instance;
	WCHAR title[MAX_LOADSTRING];
	WCHAR windowClass[MAX_LOADSTRING];
	bool rotate = false;
	bool jitter = false;
	bool mouseHeld = false;

	selected_vector_t selectedVector = selected_vector_t::NONE;
	std::tuple<float, float> vectorA;
	std::tuple<float, float> vectorB;

	ATOM registerWindowClass(HINSTANCE);
	int initInstance(HINSTANCE, int);
	LRESULT CALLBACK loop(HWND, UINT, WPARAM, LPARAM);
	void update();
}

int APIENTRY wWinMain(_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE previousInstance,
	_In_ LPWSTR    cmdLine,
	_In_ int       cmdShow)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	LoadString(instance, IDS_APP_TITLE, VectorComponents::title, MAX_LOADSTRING);
	LoadString(instance, IDC_VECTORCOMPONENTS, VectorComponents::windowClass, MAX_LOADSTRING);
	VectorComponents::registerWindowClass(instance);
	VectorComponents::grid::registerWindowClass(instance);

	if (!VectorComponents::initInstance(instance, cmdShow))
	{
		return 0;
	}

	MSG message;

	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return (int)message.wParam;
}

namespace VectorComponents {
	ATOM registerWindowClass(HINSTANCE instance)
	{
		WNDCLASS wc = {
			.style = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc = loop,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = instance,
			.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_VECTORCOMPONENTS)),
			.hCursor = LoadCursor(nullptr, IDC_ARROW),
			.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
			.lpszMenuName = MAKEINTRESOURCEW(IDC_VECTORCOMPONENTS),
			.lpszClassName = windowClass
		};

		return RegisterClass(&wc);
	}

	int initInstance(HINSTANCE _instance, int cmdShow)
	{
		instance = _instance;

		const auto window = CreateWindow(windowClass, title, WS_OVERLAPPEDWINDOW,
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

	LRESULT CALLBACK loop(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
		RECT fullRect;
		RECT gridRect;
		RECT range = { -10, -10, 10, 10 };
		GetClientRect(window, &fullRect);
		GetClientRect(window, &gridRect);
		const auto padding = 10;
		const auto controlWidth = 90;
		const auto controlHeight = 35;
		const auto left = fullRect.right - (controlWidth + padding);

		gridRect.left += padding;
		gridRect.right -= (controlWidth + padding);
		gridRect.top += padding;
		gridRect.bottom -= padding;

		switch (message)
		{
		case WM_CREATE:
		{
			HINSTANCE instance = ((CREATESTRUCT*)lParam)->hInstance;
			CreateWindow(L"button", L"A",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
				left, padding + (0 * controlHeight), controlWidth, controlHeight,
				window, (HMENU)IDC_RADIO_A, instance, nullptr);
			CreateWindow(L"button", L"B",
				WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
				left, padding + (1 * controlHeight), controlWidth, controlHeight,
				window, (HMENU)IDC_RADIO_B, instance, nullptr);
			CreateWindow(L"button", L"Rotate",
				WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				left, padding + (2 * controlHeight), controlWidth, controlHeight,
				window, (HMENU)IDC_CHECK_ROTATE, instance, nullptr);
			CreateWindow(L"button", L"Jitter",
				WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				left, padding + (3 * controlHeight), controlWidth, controlHeight,
				window, (HMENU)IDC_CHECK_JITTER, instance, nullptr);
			CreateWindow(L"GRID", L"GRID",
				WS_VISIBLE | WS_CHILD,
				gridRect.left, gridRect.top, gridRect.right - gridRect.left, gridRect.bottom - gridRect.top,
				window, (HMENU)IDC_GRID, instance, nullptr);
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
			case IDC_RADIO_A:
			{
				if (IsDlgButtonChecked(window, IDC_RADIO_A)) {
					selectedVector = selected_vector_t::VECTOR_A;
				}
				break;
			}
			case IDC_RADIO_B:
			{
				if (IsDlgButtonChecked(window, IDC_RADIO_B)) {
					selectedVector = selected_vector_t::VECTOR_B;
				}
				break;
			}
			case IDC_CHECK_ROTATE:
			{
				rotate = IsDlgButtonChecked(window, IDC_CHECK_ROTATE);
				break;
			}
			case IDC_CHECK_JITTER:
			{
				jitter = IsDlgButtonChecked(window, IDC_CHECK_JITTER);
				break;
			}
			}

			return DefWindowProc(window, message, wParam, lParam);
			break;
		}
		case WM_SIZING:
		{
			SetWindowPos(GetDlgItem(window, IDC_RADIO_A), HWND_TOP, left, padding + (0 * controlHeight), 0, 0, SWP_NOSIZE);
			SetWindowPos(GetDlgItem(window, IDC_RADIO_B), HWND_TOP, left, padding + (1 * controlHeight), 0, 0, SWP_NOSIZE);
			SetWindowPos(GetDlgItem(window, IDC_CHECK_ROTATE), HWND_TOP, left, padding + (2 * controlHeight), 0, 0, SWP_NOSIZE);
			SetWindowPos(GetDlgItem(window, IDC_CHECK_JITTER), HWND_TOP, left, padding + (3 * controlHeight), 0, 0, SWP_NOSIZE);
			SetWindowPos(GetDlgItem(window, IDC_GRID), HWND_TOP, gridRect.left, gridRect.top,
				gridRect.right - gridRect.left, gridRect.bottom - gridRect.top, 0);
			break;
		}
		case WM_TIMER:
		{
			update();

			PostMessage(GetDlgItem(window, IDC_GRID), message, wParam, lParam);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(window, message, wParam, lParam);
		}
		}
		return 0;
	}

	void drawVectors(HDC context, RECT* rect, RECT* range) {
		auto vectorASet = false;
		auto vectorBSet = false;
		if (std::get<0>(vectorA) != 0. || std::get<1>(vectorA) != 0.) {
			vectorASet = true;
		}

		if (std::get<0>(vectorB) != 0. || std::get<1>(vectorB) != 0.) {
			vectorBSet = true;
		}

		if (vectorASet) {
			SetDCPenColor(context, RGB(0, 0, 0xFF));
			SetDCBrushColor(context, RGB(0, 0, 0xFF));
			context::drawVector(context, rect, vectorA, range);
			SetDCPenColor(context, RGB(0, 0, 0));
			SetDCBrushColor(context, RGB(0, 0, 0));
			context::drawVector(context, rect, vector::normalise(vectorA), range);
			context::drawVector(context, rect, vector::perp(vectorA), range);
		}

		if (vectorBSet) {
			SetDCPenColor(context, RGB(0, 0xFF, 0));
			SetDCBrushColor(context, RGB(0, 0xFF, 0));
			context::drawVector(context, rect, vectorB, range);
			SetDCPenColor(context, RGB(0, 0, 0));
			SetDCBrushColor(context, RGB(0, 0, 0));
			context::drawVector(context, rect, vector::normalise(vectorB), range);
			context::drawVector(context, rect, vector::perp(vectorB), range);
		}

		if (vectorASet && vectorBSet) {
			SetDCPenColor(context, RGB(0xFF, 0, 0));
			SetDCBrushColor(context, RGB(0xFF, 0, 0));
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
				auto angle = atan2f(std::get<1>(vectorA), std::get<0>(vectorA));
				angle += 0.01f;
				std::get<0>(vectorA) = cosf(angle) * magnitude;
				std::get<1>(vectorA) = sinf(angle) * magnitude;
			}
			if (jitter) {
				std::get<0>(vectorA) += (((float)rand() / RAND_MAX) - 0.5f) * 0.1f;
				std::get<1>(vectorA) += (((float)rand() / RAND_MAX) - 0.5f) * 0.1f;
			}
		}


	}

	void updateVectorFromMessage(RECT* rect, LPARAM lParam) {
		const auto canvasX = GET_X_LPARAM(lParam);
		const auto canvasY = GET_Y_LPARAM(lParam);

		const auto x = util::scale((float)rect->left, (float)rect->right, (float)-10, (float)10, (float)canvasX);
		const auto y = util::scale((float)rect->top, (float)rect->bottom, (float)-10, (float)10, (float)canvasY);

		if (selectedVector == selected_vector_t::VECTOR_A) {
			std::get<0>(vectorA) = x;
			std::get<1>(vectorA) = y;
		}

		if (selectedVector == selected_vector_t::VECTOR_B) {
			std::get<0>(vectorB) = x;
			std::get<1>(vectorB) = y;
		}
	}
}