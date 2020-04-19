#pragma once

#include "BaseWindow.h"
#include "Framework.h"

class GridElement;

#include "VectorComponentsElement.h"

#define MAX_LOADSTRING 100

class GridElement : BaseWindow<GridElement, VectorComponentsElement> {
	bool mouseHeld;
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* renderTarget;
	ID2D1SolidColorBrush* brush;

public:
	ATOM RegisterWindowClass(HINSTANCE instance);
	LRESULT ProcessMessage(HWND, UINT, WPARAM, LPARAM);
};
