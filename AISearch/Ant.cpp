#include "Aco.H"


Ant::Ant(Graph* g, Parameters params, int id) 
{
	this->id = id;
	this->graph = g;
	this->params = params;
	
	tour = std::vector<int>(g->getNumberOfCities());
	std::fill(tour.begin(), tour.end(), -1);
	visitedBits = std::bitset<1024>();

	numOfVisitedCities = 0;

	// Initalise Random Number Generator
	std::random_device dev;
	gen = std::mt19937(dev());

	dis = std::uniform_real_distribution<>(0.0, 1.0);
}


void Ant::walk(PheremoneMatrix& pheremones) {
	// Generate Random Start Tour.
	auto rndTourStart = std::uniform_int_distribution<>(0, graph->getNumberOfCities()-1);
	visit(rndTourStart(gen));

	while (tour.back() == -1) {
		auto neighbour = chooseNeighbour(pheremones);

		visit(neighbour);
	}
}

int Ant::chooseNeighbour(PheremoneMatrix& pheremones) {
	float q = dis(gen);

	if (q <= 0.3) {
		// Determinstically vist the city with the greatest pheremone from current city
		return cityWithLargestPheremone(pheremones);
	}

	double p = dis(gen);
	auto neighbours = getNeighbours(pheremones, getCurrentNode());

	for (auto& neighbour : neighbours) {
		p -= computeVisitProbability(pheremones, neighbours, neighbour);

		if (p < 0) {
			return neighbour;
		}
	}

	return cityWithLargestPheremone(pheremones);
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


// Limited by exp.
inline float fastPow(float base, int exp) {
	static const int highest_bit_set[] = {
		0, 1, 2, 2, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 255, // anything past 63 is a guaranteed overflow with base > 1
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
	};

	float result = 1;

	switch (highest_bit_set[exp]) {
	case 255: // we use 255 as an overflow marker and return 0 on overflow/underflow
		if (base == 1) {
			return 1;
		}

		if (base == -1) {
			return 1 - 2 * (exp & 1);
		}

		return 0;
	case 6:
		if (exp & 1) result *= base;
		exp >>= 1;
		base *= base;
	case 5:
		if (exp & 1) result *= base;
		exp >>= 1;
		base *= base;
	case 4:
		if (exp & 1) result *= base;
		exp >>= 1;
		base *= base;
	case 3:
		if (exp & 1) result *= base;
		exp >>= 1;
		base *= base;
	case 2:
		if (exp & 1) result *= base;
		exp >>= 1;
		base *= base;
	case 1:
		if (exp & 1) result *= base;
	default:
		return result;
	}
}


inline Pheremone pheremoneTerm(PheremoneMatrix& pheremones, int alpha, int c1, int c2) {
	auto pher = pheremones.get(c1, c2);

	return fastPow(pher, alpha);
}

inline Pheremone distanceTerm(Graph* g, int beta, int c1, int c2) {
	auto weightTerm = static_cast<Pheremone>(1) / g->getWeight(c1, c2);

	return fastPow(weightTerm, beta);
}

Pheremone Ant::computeVisitProbability(PheremoneMatrix& pheremones, std::vector<int>& allowed, int proposed) {
	Pheremone normalisation = 0;
	auto current = getCurrentNode();

	for (auto& allowedNode : allowed) {
		if (allowedNode == proposed) continue;

		normalisation += pheremoneTerm(pheremones, params.Alpha, current, allowedNode)
							*distanceTerm(graph, params.Beta, current, allowedNode);
	}

	auto phTerm = pheremoneTerm(pheremones, params.Alpha, current, proposed);
	auto distTerm = distanceTerm(graph, params.Beta, current, proposed);

	return (phTerm * distTerm) / normalisation;
}

inline bool Ant::hasVisited(int city) {
	return visitedBits[city] == 1;
}

std::vector<int> Ant::getNeighbours(PheremoneMatrix& pheremones, int node) {
	std::vector<int> neighbours;
	auto currentCity = getCurrentNode();

	for (auto city = 0; city < graph->getNumberOfCities(); city++) {
		if (!hasVisited(city) && pheremones.get(currentCity, city) > 0.001) {
			neighbours.emplace_back(city);
		}
	}

	return neighbours;
}

int Ant::cityWithLargestPheremone(PheremoneMatrix& pheremones) {
	int bestCity = -1;
	Pheremone largestPheremone = -1;

	auto currentCity = getCurrentNode();
	auto neighbours = getNeighbours(pheremones, currentCity);

	for (auto& city : neighbours) {
		auto pheremone = pheremones.get(currentCity, city);
		if (pheremone >= largestPheremone) { // Actually wanna walk down it
			largestPheremone = pheremone;
			bestCity = city;
		}
	}
	
	if (bestCity == -1) {
		// Determinstically pick a city with no lower bound
		for (auto city = 0; city < graph->getNumberOfCities(); city++) {
			if (!hasVisited(city)) {
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
	visitedBits[city] = 1;

	numOfVisitedCities++;
}

void Ant::reset() {
	numOfVisitedCities = 0;
	tour[tour.size() - 1] = -1;
	visitedBits = visitedBits &= 0; // reset all to 0
}