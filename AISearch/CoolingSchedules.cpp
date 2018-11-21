#include "Annealing.h"

// Good Source: http://what-when-how.com/artificial-intelligence/a-comparison-of-cooling-schedules-for-simulated-annealing-artificial-intelligence/

// Works for any base.
float ipow(float base, int exp)
{
	float result = 1;
	for (;;)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		if (!exp)
			break;
		base *= base;
	}

	return result;
}

CoolingSchedule createExponentialMultiplicateCS(float startingTemperature, float alpha) {
	return [=](int cycle) {
		return startingTemperature * ipow(alpha, cycle);
	};
}

CoolingSchedule createQuadraticMultiplicativeCS(float startingTemperature, float alpha) {
	return [=](int cycle) {
		return startingTemperature / static_cast<float>(1 + alpha * cycle*cycle);
	};
}

CoolingSchedule createLogarithmicMultiplicativeCS(float startingTemperature, float alpha) {
	return [=](int cycle) {
		return startingTemperature / (1 + alpha * static_cast<float>(std::log(1 + cycle)));
	};
}

CoolingSchedule createGlobalOptimimumCS(float startingTemperature) {
	return [=](int cycle) {
		return startingTemperature / std::log(1 + cycle);
	};
}