#include <cmath>
#include <tuple>

#include "vector.h"

namespace vector {
	float magnitude(std::tuple<float, float> vector) {
		return std::sqrtf(std::powf(std::get<0>(vector), 2.) + std::powf(std::get<1>(vector), 2.));
	}

	std::tuple<float, float> normalise(std::tuple<float, float> vector) {
		const auto m = magnitude(vector);
		auto x = std::get<0>(vector);
		auto y = std::get<1>(vector);
		x /= m;
		y /= m;
		return { x, y };
	}
}