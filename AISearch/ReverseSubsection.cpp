#include "Annealing.h"

#include <algorithm>

ReverseSubsection::ReverseSubsection(Graph* g)
{
	std::random_device seed;

	rnd = std::mt19937(seed());
	indiciesDis = std::uniform_int_distribution<>(0, g->getNumberOfCities() - 1);
}


void reverseSubsectionWraparound(std::vector<int>& vec, int upper, int lower) {
	auto up = upper, low = lower;

	for (auto i = 0; i < upper - lower; i++) {
		std::swap(vec[low], vec[up]);

		up++;
		if (up > vec.size()-1) {
			up = 0;
		}

		low--;
		if (low < 0) {
			low = vec.size() - 1;
		}
	}
}

inline void reverseSubsection(Tour& t, int from, int to) {
	auto fromIt = t.begin() + from;
	auto toIt = t.begin() + to;

	std::reverse(fromIt, toIt);
}
void reverse(Tour& t, int from, int to) {
	if (from < to) {
		reverseSubsection(t, from, to);
	}
	else {
		reverseSubsectionWraparound(t, from, to);
	}
}

void ReverseSubsection::acceptSuccessor(Tour& s) {
	reverse(s,from, to);
}


void ReverseSubsection::createSuccessor(Tour& s) {
	from = indiciesDis(rnd);
	to = indiciesDis(rnd);

	reverse(s, from, to);
}

void ReverseSubsection::undo(Tour& s) {
	reverse(s, from, to);
}