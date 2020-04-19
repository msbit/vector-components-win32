#pragma once

#include "framework.h"

template <class Self, class Parent>
class BaseWindow {
protected:
	static LRESULT CALLBACK Loop(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Self* _this;

		if (message == WM_NCCREATE) {
			_this = (Self*)((CREATESTRUCT*)lParam)->lpCreateParams;
			SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)_this);
		}
		else {
			_this = (Self*)GetWindowLongPtr(window, GWLP_USERDATA);
		}

		if (_this) {
			return _this->ProcessMessage(window, message, wParam, lParam);
		}

		return DefWindowProc(window, message, wParam, lParam);
	}

	static ATOM Register(HINSTANCE instance, int classId, int iconId) {
		WCHAR className[100];

		LoadString(instance, classId, className, 100);

		WNDCLASS wc = {
			.style = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc = Loop,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = instance,
			.hIcon = LoadIcon(instance, MAKEINTRESOURCE(iconId)),
			.hCursor = LoadCursor(nullptr, IDC_ARROW),
			.hbrBackground = (HBRUSH)COLOR_WINDOW,
			.lpszMenuName = MAKEINTRESOURCE(classId),
			.lpszClassName = className
		};

		return RegisterClass(&wc);
	}

	Parent* getParent(HWND window) {
		HWND parentWindow = GetParent(window);
		if (parentWindow == nullptr) {
			return nullptr;
		}

		return (Parent*)GetWindowLongPtr(parentWindow, GWLP_USERDATA);
	}

	virtual LRESULT ProcessMessage(HWND, UINT, WPARAM, LPARAM) = 0;
};