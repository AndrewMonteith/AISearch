#include "Aco.H"

std::vector<Ant> createAnts(int number, Parameters& params, Graph* g) {
	number = std::max(0, std::min(number, 200));

	std::vector<Ant> ants;
	ants.reserve(number);

	for (auto i = 0; i < number; i++) {
		ants.emplace_back(g, params, i);
	}

	return ants;
}

SquareMatrix createPheremoneMatrix(Graph* g, Pheremone defaultPheremone) {
	SquareMatrix pheremones(g->getNumberOfCities(), defaultPheremone);

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

void updatePheremones(Graph* g, SquareMatrix& pheremones, Parameters& params, std::vector<Ant>& ants) {
	std::vector<Pheremone> pheremoneUpdates(ants.size());
	
	for (Ant& ant : ants) {
		pheremoneUpdates[ant.getId()] = params.Q / g->getCostOfTour(*ant.getTour());
	}

	for (auto& d : pheremones.rawBuffer()) {
		d *= params.EvaporationFactor;
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
	int bestLength = g->getCostOfTour(*ants[0].getTour()); 

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
	auto ants = createAnts(NumberOfAnts, params, g);

	auto pheremoneMatrix = createPheremoneMatrix(g, params.DefaultPheremone);
			
	for (auto i = 0; i < params.NumberOfIterations; i++) {
		resetAnts(ants);
		walkAnts(ants, pheremoneMatrix);
		updatePheremones(g, pheremoneMatrix, params, ants);
	}

	return recoverSolution(g, ants, pheremoneMatrix);
}