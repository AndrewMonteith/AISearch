#include "Aco.H"

std::vector<Ant> createAnts(int number, Graph* g) {
	number = std::max(0, std::min(number, 200));

	std::vector<Ant> ants;
	ants.reserve(number);

	for (auto i = 0; i < number; i++) {
		ants.emplace_back(g, i);
	}

	return ants;
}

SquareMatrix createPheremoneMatrix(Graph* g) {
	SquareMatrix pheremones(g->getNumberOfCities(), DefualtPheremone);

	for (auto i = 0; i < g->getNumberOfCities(); i++)
		pheremones.set(i, i, 0);

	return pheremones;
}

void walkAnts(std::vector<Ant>& ants, SquareMatrix& matrix) {
	for (auto& ant : ants) {
		ant.walk(matrix);
	}
}

void resetAnts(std::vector<Ant>& ants) {
	for (auto& ant : ants) {
		ant.reset();
	}
}

void updatePheremones(Graph* g, SquareMatrix& pheremones, std::vector<Ant>& ants) {
	std::vector<Pheremone> pheremoneUpdates(ants.size());
	
	for (Ant& ant : ants) {
		pheremoneUpdates[ant.getId()] = Q / g->getCostOfTour(*ant.getTour());
	}

	for (auto& d : pheremones.rawBuffer()) {
		d *= EvaporationFactor;
	}

	for (Ant& ant : ants) {
		std::vector<int>& tour = *ant.getTour();
		auto update = pheremoneUpdates[ant.getId()];

		for (auto i = 0; i < tour.size() - 1; i++) {
			auto from = tour[i], to = tour[i + 1];

			pheremones.update(from, to, update);
		}
	}
}

std::vector<int> recoverSolution(Graph* g, std::vector<Ant>& ants, SquareMatrix& pheremones) {
	resetAnts(ants);
	walkAnts(ants, pheremones);

	std::vector<int>* bestTour = ants[0].getTour();
	int bestLength = g->getCostOfTour(*ants[0].getTour()); // ants[0].getTourLength();

	for (auto& ant : ants) {
		auto newLength = g->getCostOfTour(*ant.getTour());
		if (newLength < bestLength) {
			bestLength = newLength;
			bestTour = ant.getTour();
		}
	}

	return *bestTour;
}


std::vector<int> ACO::solve(Graph* g) {
	auto ants = createAnts(NumberOfAnts, g);

	auto pheremoneMatrix = createPheremoneMatrix(g);

	Timer t;

	for (auto i = 0; i < NumberOfIterations; i++) {
		t.reset();
		resetAnts(ants);
		walkAnts(ants, pheremoneMatrix);
		updatePheremones(g, pheremoneMatrix, ants);
	}

	return recoverSolution(g, ants, pheremoneMatrix);
}