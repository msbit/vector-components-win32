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

	virtual LRESULT ProcessMessage(HWND, UINT, WPARAM, LPARAM) = 0;
};