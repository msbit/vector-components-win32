#include <tuple>
#include <windowsx.h>

#include "GridElement.h"
#include "VectorComponentsElement.h"
#include "framework.h"
#include "Resource.h"
#include "renderTarget.h"
#include "util.h"
#include "vector.h"

#define MAX_LOADSTRING 100

VectorComponentsElement::VectorComponentsElement(HINSTANCE instance) {
	WCHAR windowClass[MAX_LOADSTRING];

	LoadString(instance, IDC_VECTORCOMPONENTS, windowClass, MAX_LOADSTRING);

	WNDCLASS wc = {
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = Loop,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = instance,
		.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_VECTORCOMPONENTS)),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = (HBRUSH)COLOR_WINDOW,
		.lpszMenuName = MAKEINTRESOURCEW(IDC_VECTORCOMPONENTS),
		.lpszClassName = windowClass
	};

	RegisterClass(&wc);

	gridElement = new GridElement(instance);
}

VectorComponentsElement::~VectorComponentsElement() {
	delete gridElement;
}

LRESULT CALLBACK VectorComponentsElement::ProcessMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
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
	gridRect.right -= (controlWidth + (2 * padding));
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
			window, (HMENU)IDC_GRID, instance, gridElement);
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
	}
	case WM_SIZE:
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


void VectorComponentsElement::updateVectorFromMessage(RECT* rect, LPARAM lParam) {
	const auto canvasX = GET_X_LPARAM(lParam);
	const auto canvasY = GET_Y_LPARAM(lParam);

	const auto x = Util::scale((float)rect->left, (float)rect->right, (float)-10, (float)10, (float)canvasX);
	const auto y = Util::scale((float)rect->top, (float)rect->bottom, (float)-10, (float)10, (float)canvasY);

	if (selectedVector == selected_vector_t::VECTOR_A) {
		std::get<0>(vectorA) = x;
		std::get<1>(vectorA) = y;
	}

	if (selectedVector == selected_vector_t::VECTOR_B) {
		std::get<0>(vectorB) = x;
		std::get<1>(vectorB) = y;
	}
}

void VectorComponentsElement::drawVectors(ID2D1HwndRenderTarget* renderTarget, RECT* rect, RECT* range) {
	auto vectorASet = false;
	auto vectorBSet = false;
	if (std::get<0>(vectorA) != 0. || std::get<1>(vectorA) != 0.) {
		vectorASet = true;
	}

	if (std::get<0>(vectorB) != 0. || std::get<1>(vectorB) != 0.) {
		vectorBSet = true;
	}

	if (vectorASet) {
		RenderTarget::drawVector(renderTarget, rect, vectorA, range, { 0.0f, 0.0f, 1.0f });
		RenderTarget::drawVector(renderTarget, rect, Vector::normalise(vectorA), range, { 0.0f, 0.0f, 0.0f });
		RenderTarget::drawVector(renderTarget, rect, Vector::perp(vectorA), range, { 0.0f, 0.0f, 0.0f });
	}

	if (vectorBSet) {
		RenderTarget::drawVector(renderTarget, rect, vectorB, range, { 0.0f, 1.0f, 0.0f });
		RenderTarget::drawVector(renderTarget, rect, Vector::normalise(vectorB), range, { 0.0f, 0.0f, 0.0f });
		RenderTarget::drawVector(renderTarget, rect, Vector::perp(vectorB), range, { 0.0f, 0.0f, 0.0f });
	}

	if (vectorASet && vectorBSet) {
		const auto perpB = Vector::perp(vectorB);
		const auto normB = Vector::normalise(vectorB);
		const auto aPerpB = Vector::dotProduct(vectorA, perpB);
		const auto aNormB = Vector::dotProduct(vectorA, normB);
		RenderTarget::drawVector(renderTarget, rect, Vector::scalarMultiple(perpB, aPerpB), range, { 1.0f, 0.0f, 0.0f });
		RenderTarget::drawVector(renderTarget, rect, Vector::scalarMultiple(normB, aNormB), range, { 1.0f, 0.0f, 0.0f });
	}
}

void VectorComponentsElement::update() {
	const auto magnitude = Vector::magnitude(vectorA);
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