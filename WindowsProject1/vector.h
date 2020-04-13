#pragma once

#include <tuple>

namespace vector {
	float dotProduct(std::tuple<float, float>, std::tuple<float, float>);
	float magnitude(std::tuple<float, float>);
	std::tuple<float, float> normalise(std::tuple<float, float>);
	std::tuple<float, float> perp(std::tuple<float, float>);
	std::tuple<float, float> scalarMultiple(std::tuple<float, float>, float);
}