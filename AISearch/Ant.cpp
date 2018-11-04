#include "Aco.H"

Ant::Ant(Graph* g, AcoParamters* params) 
{
	this->graph = g;
	this->paramters = params;
	// Assign ant unique id
	static int ID = 0;
	id = ID;
	++ID;

	tour = std::vector<int>(g->getNumberOfCities());
	visited = std::set<int>();

	numOfVisitedCities = 0;

	// Initalise Random Number Generator
	std::random_device dev;
	gen = std::mt19937(dev());

	dis = std::uniform_real_distribution<>(0.0, 1.0);
}

void Ant::walk(SquareMatrix& pheremones) {
	// Generate Random Start Tour.
	auto rndTourStart = std::uniform_int_distribution<>(1, graph->getNumberOfCities());
	visit(rndTourStart(gen));

	while (numOfVisitedCities < graph->getNumberOfCities()) {
		visit(chooseNeighbour(pheremones));
	}
}

int Ant::chooseNeighbour(SquareMatrix& pheremones) {
	double q = dis(gen);

	if (q <= 0.2) {
		// Determinstically vist the city with the greatest pheremone from current city
		return cityWithLargestPheremone(pheremones);
	}

	double p = dis(gen);
	auto neighbours = getNeighbours(getCurrentNode());

	for (auto neighbour : neighbours) {
		p -= computeVisitProbability(pheremones, neighbours, neighbour);

		if (p < 0) {
			return neighbour;
		}
	}

	throw std::exception("failed to find neighbour");
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

inline double pheremoneTerm(SquareMatrix& pheremones, int c1, int c2) {
	return pow(pheremones.get(c1, c2), alpha);
}

const double one = 1;

inline double distanceTerm(Graph* g, int c1, int c2) {
	return pow(one / g->getWeight(c1, c2), beta);
}

double Ant::computeVisitProbability(SquareMatrix& pheremones, std::set<int>& allowed, int proposed) {
	double normalisation = 0;
	auto current = getCurrentNode();

	for (auto allowedNode : allowed) {
		if (allowedNode == proposed) continue;

		normalisation += pheremoneTerm(pheremones, current, allowedNode)*distanceTerm(graph, current, allowedNode);
	}

	auto phTerm = pheremoneTerm(pheremones, current, proposed);
	auto distTerm = distanceTerm(graph, current, proposed);

	return (phTerm * distTerm) / normalisation;
}

std::set<int> Ant::getNeighbours(int node) {
	std::set<int> neighbours;

	for (auto city = 1; city <= graph->getNumberOfCities(); city++) {
		if (visited.find(city) == visited.end()) {
			neighbours.insert(city);
		}
	}

	return neighbours;
}

int Ant::cityWithLargestPheremone(SquareMatrix& pheremones) {
	int bestCity = 0;
	double largestPheremone = -1;

	auto currentCity = getCurrentNode();

	for (int city : getNeighbours(currentCity)) {
		auto pheremone = pheremones.get(currentCity, city);
		if (pheremone >= largestPheremone) {
			largestPheremone = pheremone;
			bestCity = city;
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
	visited.clear();
}

Ant::Ant()
	: graph(nullptr), paramters(nullptr) {}