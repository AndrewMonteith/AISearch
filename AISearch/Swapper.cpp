#include "Annealing.h"

Swapper::Swapper(Graph* g) 
{
	std::random_device seed;

	rnd = std::mt19937(seed());
	indiciesDis = std::uniform_int_distribution<>(0, g->getNumberOfCities() - 1);
}

inline void swap(Tour& s, int from, int to) {
	int temp = s[from];
	s[from] = s[to];
	s[to] = temp;
}

void Swapper::createSuccessor(Tour& s) {
	from = indiciesDis(rnd);
	to = indiciesDis(rnd);

	swap(s, from, to);
}

void Swapper::acceptSuccessor(Tour& s) {
	swap(s, from, to);
}

void Swapper::undo(Tour& s) {
	swap(s, from, to);
}