#include "Annealing.h"

Swapper::Swapper(Graph* g) 
{
	std::random_device seed;

	rnd = std::mt19937(seed());
	indiciesDis = std::uniform_int_distribution<>(0, g->getNumberOfCities() - 1);
}

void Swapper::createSuccessor(Tour& s) {
	from = indiciesDis(rnd);
	to = indiciesDis(rnd);

	std::swap(s[from], s[to]);
}

void Swapper::acceptSuccessor(Tour& s) {
	std::swap(s[from], s[to]);
}

void Swapper::undo(Tour& s) {
	std::swap(s[from], s[to]);
}