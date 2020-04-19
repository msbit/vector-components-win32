#pragma once

#include "framework.h"

template <class Self, class Parent>
class BaseWindow {
protected:
	static LRESULT CALLBACK Loop(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT ProcessMessage(HWND, UINT, WPARAM, LPARAM) = 0;
};