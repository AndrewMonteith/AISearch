#include "Annealing.h"

#include <algorithm>

Tour createInitalState(int numberOfNodes) {
	std::vector<int> numbers(numberOfNodes);

	// Fill vector with numbers from 1 to number of cities.
	for (auto i = 0; i < numberOfNodes; i++) {
		numbers[i] = i;
	}

	std::random_shuffle(numbers.begin(), numbers.end());

	return numbers;
}

inline double acceptanceProbability(int currentEnergy, int newEnergy, double temperature) {
	if (newEnergy < currentEnergy) { // Improvment?
		return 1.0;
	}

	double DE = currentEnergy - newEnergy;
	double exponent = DE / temperature;

	return exp(exponent); // Randomly Choose Successor
}

inline float nonMonotonicCoolingSchedule(int optimallyFoundFitness, int currentFitness) {
	return (1 + static_cast<float>(currentFitness - optimallyFoundFitness)/currentFitness);
}

std::vector<int> Annealing::solve(Graph* g) {
	Tour current = createInitalState(g->getNumberOfCities());
	auto currentCost = g->getCostOfTour(current);
	
	float temperature = coolingSchedule(0);
	int cycle = 0;

	// int bestFoundFitness = g->getCostOfTour(current); // used for non-monotonic cooling.

	while (temperature > 0.01) { // magic value but tends to work.
		successorGenerator->createSuccessor(current); // generate next successor
	
		auto newCost = g->getCostOfTour(current);

		auto acceptProbability = acceptanceProbability(currentCost, newCost, temperature);

		bool keepSuccessor = propabilityDis(rnd) < acceptProbability;

		if (!keepSuccessor) {
			successorGenerator->undo(current);
		}
		else {
			currentCost = newCost;
		}

		cycle += 1;

		temperature = coolingSchedule(cycle); // *nonMonotonicCoolingSchedule(bestFoundFitness, newCost);
		
		// bestFoundFitness = std::min(newCost, bestFoundFitness);
	}

	return current;
}

void Annealing::setSuccessorGenerator(SuccessorGenerator* sg)
{
	this->successorGenerator = sg;
}


Annealing::Annealing(CoolingSchedule cs)
	: coolingSchedule(cs), successorGenerator(nullptr)
{
	std::random_device seed;

	rnd = std::mt19937(seed());
	propabilityDis = std::uniform_real_distribution<>(0, 1);
}


Annealing::~Annealing()
{
}
