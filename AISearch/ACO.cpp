#include "Aco.H"
#include <iostream>
#include <algorithm>

std::vector<Ant> createAnts(int number, Parameters& params, Graph* g) {
	// number = std::max(0, std::min(number, 200));
	std::cout << "Number:" << number << std::endl;
	std::vector<Ant> ants;
	ants.reserve(number);

	for (auto i = 0; i < number; i++) {
		ants.emplace_back(g, params, i);
	}

	return ants;
}

PheremoneMatrix createPheremoneMatrix(Graph* g, Pheremone defaultPheremone) {
	PheremoneMatrix pheremones(g->getNumberOfCities(), defaultPheremone);

	for (auto i = 0; i < g->getNumberOfCities(); i++)
		pheremones.set(i, i, 0);

	return pheremones;
}

void walkAnts(std::vector<Ant>& ants, PheremoneMatrix& matrix) {
	for (auto& ant : ants) {
		ant.walk(matrix);
	}
}

void resetAnts(std::vector<Ant>& ants) {
	for (auto& ant : ants) {
		ant.reset();
	}
}

#include <iostream>

void updatePheremonesRankBased(Graph* g, PheremoneMatrix& pheremones, Parameters& params, std::vector<Ant>& ants, int w = 30) {
	std::vector<int> tourLengths(ants.size());
	for (auto& ant : ants) {
		tourLengths[ant.getId()] = g->getCostOfTour(*ant.getTour());
	}

	// Sort Ants based of length of tour.
	std::sort(ants.begin(), ants.end(), 
		[&](Ant& ant1, Ant& ant2) -> bool {
			return tourLengths[ant1.getId()] < tourLengths[ant2.getId()];
		});

	std::vector<Pheremone> pheremoneUpdates(w);
	for (auto r = 0; r < w; r++) {
		auto ant = ants[r];

		pheremoneUpdates[r] = (w-r)*params.Q / tourLengths[ant.getId()];
	}

	for (auto& d : pheremones.rawBuffer()) {
		d *= params.EvaporationFactor;
	}
	
	for (auto r = 0; r < w; r++) {
		auto ant = ants[r];
		auto tour = *ant.getTour();

		for (auto i = 0; i < tour.size() - 1; i++) {
			auto from = tour[i], to = tour[i + 1];

			pheremones.update(from, to, pheremoneUpdates[r]);
		}
	}
}

void updatePheremones(Graph* g, PheremoneMatrix& pheremones, Parameters& params, std::vector<Ant>& ants) {
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

std::vector<int> recoverSolution(Graph* g, std::vector<Ant>& ants, PheremoneMatrix& pheremones) {
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
		updatePheremonesRankBased(g, pheremoneMatrix, params, ants, 50);
	}

	return recoverSolution(g, ants, pheremoneMatrix);
}