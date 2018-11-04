#include "Annealing.h"

Swapper::Swapper(Graph* g) 
	: SuccessorGenerator(g) 
{
	std::random_device seed;

	rnd = std::mt19937(seed());
	indiciesDis = std::uniform_int_distribution<>(0, g->getNumberOfCities() - 1);
}

inline void swap(State& s, int from, int to) {
	int temp = s[from];
	s[from] = s[to];
	s[to] = temp;
}

void Swapper::createSuccessor(State& s) {
	from = indiciesDis(rnd);
	to = indiciesDis(rnd);

	swap(s, from, to);
}

void Swapper::acceptSuccessor(State& s) {
	swap(s, from, to);
}

void Swapper::undo(State& s) {
	swap(s, from, to);
}