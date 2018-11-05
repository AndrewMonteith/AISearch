#pragma once

#include "TourSolver.h"

#include <set>

typedef double Pheremone;

// Paramters used for the simulation
const Pheremone EvaporationFactor = 0.6;
// const Pheremone alpha = 3; // , beta = 6; Change by changing how the numbers are multiplied in pheremoneTerm and distanceTerm functions
const Pheremone Q = 15;
const Pheremone DefualtPheremone = 0.5;
const int NumberOfIterations = 50;

class SquareMatrix {
	// Matrix maintains the symmetric property.
public:
	SquareMatrix(int dimension, Pheremone defaultValue = 0)
	{
		this->dimension = dimension;
		buffer = std::vector<Pheremone>(dimension*dimension, defaultValue);
	}
	
	Pheremone get(int x, int y) {
		return buffer[flatten(x, y)];
	}

	void set(int x, int y, Pheremone value) {
		buffer[flatten(x, y)] = value;
		buffer[flatten(y, x)] = value;
	}

	inline int flatten(int x, int y) {
		return y*dimension + x;
	}

	std::vector<Pheremone>& rawBuffer() {
		return buffer;
	}

	void update(int x, int y, Pheremone update) {
		buffer[flatten(x, y)] += update;
		buffer[flatten(y, x)] += update;
	}

private:
	int dimension;
	std::vector<Pheremone> buffer;
};

class Ant {
public:
	Ant(Graph* g,  int id);
	Ant();

	void reset();
	void walk(SquareMatrix& pheremones);
	bool walkedEdge(int c1, int c2);

	std::vector<int>* getTour();
	int getId();

//	Ant();
private:
	int getCurrentNode();
	void visit(int city);

	int chooseNeighbour(SquareMatrix& pheremones);
	int cityWithLargestPheremone(SquareMatrix& pheremones);
	Pheremone computeVisitProbability(SquareMatrix& pheremones, std::vector<int>& allowed, int city);
	std::vector<int> getNeighbours(SquareMatrix& pheremones, int node);

	std::vector<int> tour;
	std::set<int> visited; // used for quick lookup time

	int numOfVisitedCities;

	int id; // used for precomputing uniquely identifying any precomputed vals

	Graph* graph;

	// Random Number Generator Stuff
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;
};

class ACO : public TourSolver {
public:
	std::vector<int> solve(Graph* graph);

	ACO(int numberOfAnts)
		: NumberOfAnts(numberOfAnts) {}
private:
	int NumberOfAnts;
};