#include "Aco.H"

std::vector<Ant> createAnts(int number, Graph* g, AcoParamters* paramters) {
	number = std::max(0, std::min(number, 200));

	std::vector<Ant> ants(number);

	for (auto i = 0; i < number; i++) {
		ants[i] = (Ant(g, paramters, i));
	}

	return ants;
}

SquareMatrix createPheremoneMatrix(Graph* g) {
	SquareMatrix pheremones(g->getNumberOfCities(), DefualtPheremone);

	for (auto i = 1; i <= g->getNumberOfCities(); i++)
		pheremones.set(i, i, 0);

	return pheremones;
}

void walkAnts(std::vector<Ant>& ants, SquareMatrix& matrix) {
	for (Ant& ant : ants) {
		ant.walk(matrix);
	}
}

void resetAnts(std::vector<Ant>& ants) {
	for (Ant& ant : ants) {
		ant.reset();
	}
}

void updatePheremones(Graph* g, SquareMatrix& pheremones, std::vector<Ant>& ants) {
	std::vector<double> pheremoneUpdates(ants.size());

	for (Ant& ant : ants) {
		pheremoneUpdates[ant.getId()] = Q / g->getCostOfTour(*ant.getTour());
	}

	// n * n * n = n^3
	for (auto c1 = 1; c1 <= g->getNumberOfCities(); c1++) {
		for (auto c2 = c1+1; c2 <= g->getNumberOfCities(); c2++) {
			// Compute Change in pheremone
			double changeInPheremone = 0;

			for (Ant& ant : ants) { // ants = std::vector<Ant>&
				if (ant.walkedEdge(c1, c2)) {
					changeInPheremone += pheremoneUpdates[ant.getId()]; //->getCostOfTour(*ant.getTour());  //// pheremoneUpdates[ant.getId()];
				}
			}

			pheremones.set(c1, c2, EvaporationFactor*pheremones.get(c1, c2) + changeInPheremone);
		}
	}
}

std::vector<int> recoverSolution(Graph* g, std::vector<Ant>& ants, SquareMatrix& pheremones) {
	resetAnts(ants);
	walkAnts(ants, pheremones);

	std::vector<int>* bestTour = ants[0].getTour();
	int bestLength = g->getCostOfTour(*ants[0].getTour()); // ants[0].getTourLength();

	for (Ant& ant : ants) {

		auto newLength = g->getCostOfTour(*ant.getTour());
		if (newLength < bestLength) {
			bestLength = newLength;
			bestTour = ant.getTour();
		}
	}

	return *bestTour;
}

AcoParamters createParamters() {
	std::random_device rnd;

	std::mt19937 gen(rnd());

	std::uniform_real_distribution<> evaporationFactorDis(0.4, 0.6);
	std::uniform_real_distribution<> betaDis(6, 9);
	std::uniform_int_distribution<> numberDis(50, 90);

	AcoParamters paramters;

	paramters.beta = betaDis(gen);
	paramters.evaporationFactor = 0.6; // evaporationFactorDis(gen);
	paramters.NumberOfIterations = numberDis(gen);

	return paramters;
}

std::vector<int> ACO::solve(Graph* g) {
	auto paramters = createParamters();

	auto ants = createAnts(NumberOfAnts, g, &paramters);

	auto pheremoneMatrix = createPheremoneMatrix(g);

	Timer t;

	for (auto i = 0; i < paramters.NumberOfIterations; i++) {
		t.reset();
		resetAnts(ants);
		walkAnts(ants, pheremoneMatrix);
		updatePheremones(g, pheremoneMatrix, ants);
	}

	return recoverSolution(g, ants, pheremoneMatrix);
}