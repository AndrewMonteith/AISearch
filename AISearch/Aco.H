#pragma once

#include "TourSolver.h"

// Paramters used for the simulation
const double EvaporationFactor = 0.6;
const double alpha = 3, beta = 6.5;
const double Q = 15;
const double DefualtPheremone = 0.5;

class SquareMatrix {
	// Matrix maintains the symmetric property.
public:
	SquareMatrix(int dimension, double defaultValue = 0)
	{
		this->dimension = dimension;
		buffer = std::vector<double>(dimension*dimension, defaultValue);
	}

	double get(int x, int y) {
		return buffer[flatten(x, y)];
	}

	void set(int x, int y, double value) {
		buffer[flatten(x, y)] = value;
		buffer[flatten(y, x)] = value;
	}

	inline int flatten(int x, int y) {
		return (x - 1)*dimension + (y - 1);
	}

	void print() {
		for (auto x = 1; x <= dimension; x++)
		{
			for (auto y = 1; y <= dimension; y++) {
				std::cout << get(x, y) << " ";
			}
			std::cout << std::endl;
		}
	}

private:
	int dimension;
	std::vector<double> buffer;
};

struct AcoParamters {
	double beta;
	double evaporationFactor;
	int NumberOfIterations;
};

class Ant {
public:
	Ant(Graph* g, AcoParamters* ap);
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
	double computeVisitProbability(SquareMatrix& pheremones, std::set<int>& allowed, int city);
	std::set<int> getNeighbours(int node);

	AcoParamters* paramters;
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