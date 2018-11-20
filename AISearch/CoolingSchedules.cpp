#include "Annealing.h"

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

CoolingSchedule createLinearCoolingSchedule(float startingTemperature, float alpha) {
	return [=](int cycle) {
		return startingTemperature * ipow(alpha, cycle);
	};
}
