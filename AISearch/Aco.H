#pragma once

#include "TourSolver.h"
#include "Utils.hpp"

#include <set>
#include <bitset>

typedef float Pheremone;
typedef SquareMatrix<Pheremone> PheremoneMatrix;


// Paramters used for the simulation
struct Parameters {
	Pheremone EvaporationFactor;
	int Alpha;
	int Beta;
	Pheremone Q;
	Pheremone DefaultPheremone;
	int NumberOfIterations;
};


class Ant {
public:
	Ant(Graph* g, Parameters params, int id);
	
	void reset();
	void walk(PheremoneMatrix& pheremones);
	bool walkedEdge(int c1, int c2);

	std::vector<int>* getTour();
	int getId();
private:
	int getCurrentNode();
	void visit(int city);

	int chooseNeighbour(PheremoneMatrix& pheremones);
	int cityWithLargestPheremone(PheremoneMatrix& pheremones);
	Pheremone computeVisitProbability(PheremoneMatrix& pheremones, std::vector<int>& allowed, int city);
	std::vector<int> getNeighbours(PheremoneMatrix& pheremones, int node);
	inline bool hasVisited(int city);


	std::vector<int> tour;
	std::bitset<1024> visitedBits;

	int numOfVisitedCities; 
	int id; // used for precomputing uniquely identifying any precomputed vals

	Parameters params;

	Graph* graph;

	// Random Number Generator Stuff
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;
};

class ACO : public TourSolver {
public:
	std::vector<int> solve(Graph* graph);

	ACO(int numberOfAnts, Parameters params)
		: NumberOfAnts(numberOfAnts), params(params) {}
private:
	int NumberOfAnts;
	Parameters params;
};