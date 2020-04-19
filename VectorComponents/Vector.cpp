#include <cmath>
#include <tuple>

#include "Vector.h"

namespace Vector {
	float dotProduct(std::tuple<float, float> a, std::tuple<float, float> b) {
		return std::get<0>(a) * std::get<0>(b) + std::get<1>(a) * std::get<1>(b);
	}

	float magnitude(std::tuple<float, float> vector) {
		return std::sqrtf(std::powf(std::get<0>(vector), 2.) + std::powf(std::get<1>(vector), 2.));
	}

	std::tuple<float, float> normalise(std::tuple<float, float> vector) {
		const auto m = magnitude(vector);
		return { std::get<0>(vector) / m, std::get<1>(vector) / m };
	}

	std::tuple<float, float> perp(std::tuple<float, float> vector) {
		const auto x = std::get<0>(vector);
		const auto y = std::get<1>(vector);
		return normalise({ y, -x });
	}

	std::tuple<float, float> scalarMultiple(std::tuple<float, float> vector, float scalar) {
		return { std::get<0>(vector) * scalar, std::get<1>(vector) * scalar };
	}
}