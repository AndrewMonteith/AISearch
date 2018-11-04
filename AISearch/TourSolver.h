#pragma once

#include "stdinc.h"

class TourSolver {
public:
	virtual std::vector<int> solve(Graph* t) = 0;
};