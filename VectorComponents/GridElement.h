#pragma once

#include "BaseWindow.h"
#include "Framework.h"

class GridElement;

#include "VectorComponentsElement.h"

#define MAX_LOADSTRING 100

class GridElement : BaseWindow<GridElement, VectorComponentsElement> {
	bool mouseHeld = false;
	ID2D1Factory* factory = nullptr;
	ID2D1HwndRenderTarget* renderTarget = nullptr;
	ID2D1SolidColorBrush* brush = nullptr;

public:
	GridElement(HINSTANCE);
	LRESULT ProcessMessage(HWND, UINT, WPARAM, LPARAM);
};
