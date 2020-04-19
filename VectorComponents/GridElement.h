#pragma once

#include "Framework.h"

#define MAX_LOADSTRING 100

class GridElement {
	bool mouseHeld;
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* renderTarget;
	ID2D1SolidColorBrush* brush;

public:
	ATOM RegisterWindowClass(HINSTANCE instance);

private:
	static LRESULT CALLBACK Loop(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK ProcessMessage(HWND, UINT, WPARAM, LPARAM);
};