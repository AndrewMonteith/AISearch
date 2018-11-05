#include "Aco.H"

Ant::Ant(Graph* g, int id) 
{
	this->id = id;
	this->graph = g;
	
	tour = std::vector<int>(g->getNumberOfCities());
	std::fill(tour.begin(), tour.end(), -1);
	visited = std::set<int>();

	numOfVisitedCities = 0;

	// Initalise Random Number Generator
	std::random_device dev;
	gen = std::mt19937(dev());

	dis = std::uniform_real_distribution<>(0.0, 1.0);
}

void Ant::walk(SquareMatrix& pheremones) {
	// Generate Random Start Tour.
	auto rndTourStart = std::uniform_int_distribution<>(0, graph->getNumberOfCities()-1);
	visit(rndTourStart(gen));

	while (tour.back() == -1) {
		visit(chooseNeighbour(pheremones));
	}
}

int Ant::chooseNeighbour(SquareMatrix& pheremones) {
	double q = dis(gen);

	if (q <= 0.3) {
		// Determinstically vist the city with the greatest pheremone from current city
		return cityWithLargestPheremone(pheremones);
	}

	double p = dis(gen);
	auto neighbours = getNeighbours(pheremones, getCurrentNode());

	for (int neighbour : neighbours) {
		p -= computeVisitProbability(pheremones, neighbours, neighbour);

		if (p < 0) {
			return neighbour;
		}
	}



	return cityWithLargestPheremone(pheremones);

	// throw std::exception("failed to find neighbour");
}

bool Ant::walkedEdge(int c1, int c2) {
	auto numOfCities = graph->getNumberOfCities();

	for (auto i = 0; i < numOfCities-1; i++) {
		auto from = tour[i], to = tour[i + 1];

		if ((from == c1 && to == c2) || (from == c2 && to == c1)) { // symmetric graph
			return true;
		}
	}

	return (tour[0] == c1 && tour[numOfCities-1] == c2) || (tour[0] == c2 && tour[numOfCities-1] == c1);
}

std::vector<int>* Ant::getTour() {
	return &tour;
}

int Ant::getId() {
	return id;
}

inline Pheremone pheremoneTerm(SquareMatrix& pheremones, int c1, int c2) {
	auto pher = pheremones.get(c1, c2);

	return pher * pher*pher;
	// return pow(pheremones.get(c1, c2), alpha);
}

inline Pheremone distanceTerm(Graph* g, int c1, int c2) {
	auto weightTerm = static_cast<Pheremone>(1) / g->getWeight(c1, c2);

	 return weightTerm * weightTerm*weightTerm*weightTerm*weightTerm*weightTerm;
	// return pow(static_cast<Pheremone>(1) / g->getWeight(c1, c2), beta);
}

Pheremone Ant::computeVisitProbability(SquareMatrix& pheremones, std::vector<int>& allowed, int proposed) {
	Pheremone normalisation = 0;
	auto current = getCurrentNode();

	for (auto allowedNode : allowed) {
		if (allowedNode == proposed) continue;

		normalisation += pheremoneTerm(pheremones, current, allowedNode)*distanceTerm(graph, current, allowedNode);
	}

	auto phTerm = pheremoneTerm(pheremones, current, proposed);
	auto distTerm = distanceTerm(graph, current, proposed);

	return (phTerm * distTerm) / normalisation;
}

#include <iostream>

std::vector<int> Ant::getNeighbours(SquareMatrix& pheremones, int node) {
	std::vector<int> neighbours;
	auto currentCity = getCurrentNode();

	for (auto city = 0; city < graph->getNumberOfCities(); city++) {
		if (visited.find(city) == visited.end() && pheremones.get(currentCity, city) > 0.001) {
			neighbours.push_back(city);
		}
	}

	return neighbours;
}

int Ant::cityWithLargestPheremone(SquareMatrix& pheremones) {
	int bestCity = -1;
	double largestPheremone = -1;

	auto currentCity = getCurrentNode();
	auto neighbours = getNeighbours(pheremones, currentCity);

	for (int city : neighbours) {
		auto pheremone = pheremones.get(currentCity, city);
		if (pheremone >= largestPheremone) { // Actually wanna walk down it
			largestPheremone = pheremone;
			bestCity = city;
		}
	}
	
	if (bestCity == -1) {
		// Determinstically pick a city with no lower bound
		for (auto city = 0; city < graph->getNumberOfCities(); city++) {
			if (visited.find(city) == visited.end()) {
				return city;
			}
		}
	}
	

	return bestCity;
}

int Ant::getCurrentNode() {
	return tour[numOfVisitedCities - 1];
}

void Ant::visit(int city) {
	tour[numOfVisitedCities] = city;
 	visited.insert(city);
	
	numOfVisitedCities++;
}

void Ant::reset() {
	numOfVisitedCities = 0;
	tour[tour.size() - 1] = -1;
	visited.clear();
}

Ant::Ant()
	: graph(nullptr) {}