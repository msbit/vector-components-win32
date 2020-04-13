#pragma once

#include <tuple>

namespace vector {
	float magnitude(std::tuple<float, float>);
	std::tuple<float, float> normalise(std::tuple<float, float>);
}