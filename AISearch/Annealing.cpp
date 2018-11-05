#include "Annealing.h"

#include <algorithm>

Tour createInitalState(int numberOfNodes) {
	std::vector<int> numbers(numberOfNodes);

	// Fill vector with numbers from 1 to number of cities.
	for (auto i = 0; i < numberOfNodes; i++) {
		numbers[i] = i+1;
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

std::vector<int> Annealing::solve(Graph* g) {
	// Allocate 2 vectors to act as current and successor
	Tour current = createInitalState(g->getNumberOfCities());
	Tour next = current; // Copy
	
	double temperature = startingTemperature; 
	while (temperature > 0.01) { // magic value but tends to work.
		successorGenerator->createSuccessor(next); // generate next successor

		auto acceptProbability = acceptanceProbability(g->getCostOfTour(current), g->getCostOfTour(next), temperature);

		bool keepSuccessor = propabilityDis(rnd) < acceptProbability;

		if (keepSuccessor) {
			successorGenerator->acceptSuccessor(current); // perform same transition to current
		}
		else {
			successorGenerator->undo(next);
		}

		temperature *= (1-alpha);
	}

	return current;
}

void Annealing::setSuccessorGenerator(SuccessorGenerator* sg)
{
	this->successorGenerator = sg;
}


Annealing::Annealing(double alpha, double startingTemp)
	:alpha(alpha), startingTemperature(startingTemp)
{
	std::random_device seed;

	rnd = std::mt19937(seed());
	propabilityDis = std::uniform_real_distribution<>(0, 1);
}


Annealing::~Annealing()
{
}
