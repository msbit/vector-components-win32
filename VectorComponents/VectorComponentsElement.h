#pragma once

#include <tuple>

#include "BaseWindow.h"
#include "Framework.h"
#include "GridElement.h"

enum class selected_vector_t { NONE, VECTOR_A, VECTOR_B };

class VectorComponentsElement : BaseWindow<VectorComponentsElement, ATOM> {
	selected_vector_t selectedVector = selected_vector_t::NONE;
	GridElement* gridElement = nullptr;
	bool rotate = false;
	bool jitter = false;

	std::tuple<float, float> vectorA;
	std::tuple<float, float> vectorB;
public:
	VectorComponentsElement(HINSTANCE);
	~VectorComponentsElement();

	void updateVectorFromMessage(RECT*, LPARAM);
	void drawVectors(ID2D1HwndRenderTarget*, RECT*, RECT*);
	LRESULT CALLBACK ProcessMessage(HWND, UINT, WPARAM, LPARAM);

private:
	void update();
};