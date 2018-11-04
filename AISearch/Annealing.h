#pragma once

#include "TourSolver.h"

typedef std::vector<int> State;

class SuccessorGenerator
{
public:
	SuccessorGenerator();
	SuccessorGenerator(Graph* g);
	~SuccessorGenerator();

	virtual void createSuccessor(State& s) = 0; // generate a new successor
	virtual void acceptSuccessor(State& s) = 0; // accept the successor.
	virtual void undo(State& s) = 0; // Optimisation wise we need to make all successors transformablew
private:
	Graph* g;
};

class Annealing // : public TourSolver
{
public:
	std::vector<int> solve(Graph* g);
	void setSuccessorGenerator(SuccessorGenerator* sg);

	Annealing(double alpha, double startingTemp);
	~Annealing();
private:
	// factor by which the temperature cools
	double alpha; 
	
	// starting temperature for annealing process
	double startingTemperature;

	SuccessorGenerator* successorGenerator;

	// all the random stuff
	std::mt19937 rnd;
	std::uniform_real_distribution<> propabilityDis;
};


class Swapper : public SuccessorGenerator {
public:
	Swapper(Graph* g);

	void createSuccessor(State& s);
	void undo(State& s);
	void acceptSuccessor(State& s);
private:
	int from = 0, to = 0;
	std::uniform_int_distribution<> indiciesDis;
	std::mt19937 rnd;
};

class ReverseSubsection : public SuccessorGenerator {
public:
	ReverseSubsection(Graph* g);
	void createSuccessor(State& s);
	void undo(State& s);
	void acceptSuccessor(State& s);
private:
	State::iterator from, to; // 2 iterators that we reveresed inbetween
	std::uniform_int_distribution<> indiciesDis;
	std::mt19937 rnd;
};