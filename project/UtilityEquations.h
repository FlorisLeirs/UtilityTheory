#pragma once
#include <corecrt_math.h>

namespace Utility
{
	inline float Linear(const float input, const float maxValue) // actually just normalization
	{
		return input / maxValue;
	}

	inline float Quadratic(const float input, const float maxValue, const float exp) // exp [0,1] -> rotated parabola 
	{
		return powf(Linear(input, maxValue), exp);
	}

	inline float Logistic(const float a, const float x, const float b) // a makes transition in graph rougher or smoother
	{
		return 1 / (1 + powf(2.718281828f * a, -x + b)); 
	}
}


