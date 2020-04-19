#pragma once

#include <tuple>

#include "GridElement.h"
#include "framework.h"

enum class selected_vector_t { NONE, VECTOR_A, VECTOR_B };

class VectorComponentsElement {
	selected_vector_t selectedVector = selected_vector_t::NONE;
	GridElement* gridElement;
	bool rotate = false;
	bool jitter = false;

	std::tuple<float, float> vectorA;
	std::tuple<float, float> vectorB;
public:
	VectorComponentsElement(HINSTANCE);
	~VectorComponentsElement();

	ATOM RegisterWindowClass(HINSTANCE);
	void updateVectorFromMessage(RECT*, LPARAM);
	void drawVectors(ID2D1HwndRenderTarget*, RECT*, RECT*);

private:
	static LRESULT CALLBACK Loop(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK ProcessMessage(HWND, UINT, WPARAM, LPARAM);
	void update();
};