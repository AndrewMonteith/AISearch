#include "Annealing.h"

#include <algorithm>

ReverseSubsection::ReverseSubsection(Graph* g)
{
	std::random_device seed;

	rnd = std::mt19937(seed());
	indiciesDis = std::uniform_int_distribution<>(0, g->getNumberOfCities() - 1);
}


void ReverseSubsection::acceptSuccessor(Tour& s) {
	std::reverse(from, to);
}

void ReverseSubsection::createSuccessor(Tour& s) {
	int rndIndex1 = indiciesDis(rnd);
	int rndIndex2 = indiciesDis(rnd);

	from = s.begin() + std::min(rndIndex1, rndIndex2);
	to = s.begin() + std::max(rndIndex1, rndIndex2);

	std::reverse(from, to);
}

void ReverseSubsection::undo(Tour& s) {
	std::reverse(from, to);
}