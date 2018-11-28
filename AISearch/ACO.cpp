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
	// Only select the `w` best ants to update the pheremone.
	w = std::min(static_cast<int>(ants.size() - 1), w);

	std::vector<int> tourLengths(ants.size());
	for (auto& ant : ants) {
		tourLengths[ant.getId()] = g->getCostOfTour(ant.getTour());
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
		auto tour = ant.getTour();

		for (auto i = 0; i < tour.size() - 1; i++) {
			auto from = tour[i], to = tour[i + 1];

			pheremones.update(from, to, pheremoneUpdates[r]);
		}
	}
}

struct MmasParamters {
	bool useGlobal;
	int globalBestTour;
	int iterationCount;
};

inline bool shouldFlipUseGlobal(int iterationCount) {
	if (iterationCount < 25) {
		return false;
	}
	else if (iterationCount < 75) {
		return iterationCount % 5 == 0;
	}
	else if (iterationCount < 125) {
		return iterationCount % 3 == 0;
	}
	else if (iterationCount < 250) {
		return iterationCount % 2 == 0;
	}
	else {
		return true;
	}
}

void updatePheremonesMinMax(Graph* g, PheremoneMatrix& pheremones, Parameters& params, std::vector<Ant>& ants, MmasParamters& mmasParams) {
	auto bestAnt = std::min_element(ants.begin(), ants.end(), [&](Ant& ant1, Ant& ant2) { 
		return g->getCostOfTour(ant1.getTour()) < g->getCostOfTour(ant2.getTour()); 
	});

	auto bestFoundTour = bestAnt->getTour();
	auto bestTourLen = g->getCostOfTour(bestFoundTour);


	if (shouldFlipUseGlobal(mmasParams.iterationCount)) {
		mmasParams.useGlobal = !mmasParams.useGlobal;
	}
	++mmasParams.iterationCount;
	mmasParams.globalBestTour = std::min(mmasParams.globalBestTour, bestTourLen);

	auto change = static_cast<double>(1) / (mmasParams.useGlobal ? mmasParams.globalBestTour : bestTourLen);

	auto pherMax = static_cast<double>(0.8); // static_cast<double>(1 / (1 - params.EvaporationFactor)) * static_cast<double>(1 / bestTourLen);
	auto pherMin = static_cast<double>(0.05); // pherMax / (2 * g->getNumberOfCities());

	for (auto& d : pheremones.rawBuffer()) {
		d = std::max(pherMin, d * params.EvaporationFactor);
	}

	for (auto i = 0; i < bestFoundTour.size() - 1; ++i) {
		auto from = bestFoundTour[i], to = bestFoundTour[i + 1];
		
		auto curPheremone = pheremones.get(from, to);
		curPheremone = std::min(pherMax, std::max(curPheremone+change, pherMin));
		pheremones.set(from, to, curPheremone);
	}
}

void updatePheremones(Graph* g, PheremoneMatrix& pheremones, Parameters& params, std::vector<Ant>& ants) {
	std::vector<Pheremone> pheremoneUpdates(ants.size());
	
	for (Ant& ant : ants) {
		pheremoneUpdates[ant.getId()] = params.Q / g->getCostOfTour(ant.getTour());
	}

	for (auto& d : pheremones.rawBuffer()) {
		d *= params.EvaporationFactor;
	}

	for (Ant& ant : ants) {
		std::vector<int>& tour = ant.getTour();
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

	std::vector<int>* bestTour = &ants[0].getTour();
	int bestLength = g->getCostOfTour(ants[0].getTour()); 

	for (auto& ant : ants) {
		auto newLength = g->getCostOfTour(ant.getTour());
		if (newLength < bestLength) {
			bestLength = newLength;
			bestTour = &ant.getTour();
		}
	}

	return *bestTour;
}

std::vector<int> ACO::solve(Graph* g) {
	auto ants = createAnts(NumberOfAnts, params, g);
	auto pheremoneMatrix = createPheremoneMatrix(g, params.DefaultPheremone);
	
	MmasParamters mmasParams {
		false, 10000000000, 0
	};

	Timer t;
	for (auto i = 0; i < params.NumberOfIterations; i++) {
		std::cout << t.elapsed() << std::endl;
		resetAnts(ants);
		walkAnts(ants, pheremoneMatrix);
		updatePheremonesMinMax(g, pheremoneMatrix, params, ants, mmasParams);
	}

	return recoverSolution(g, ants, pheremoneMatrix);
}