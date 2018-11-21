#pragma once

#ifndef FOOBAR_H
#define FOOBAR_H
#include "TourSolver.h"
#include "Utils.hpp"

typedef std::vector<int> Tour;
typedef std::function<float(int cycle)> CoolingSchedule; // <return-temp>(cycle)

CoolingSchedule createExponentialMultiplicateCS(float startingTemp, float alpha);
CoolingSchedule createQuadraticMultiplicativeCS(float startingTemp, float alpha);
CoolingSchedule createLogarithmicMultiplicativeCS(float startingTemp, float alpha);
CoolingSchedule createGlobalOptimimumCS(float startingTemp);

class SuccessorGenerator
{
public:
	virtual void createSuccessor(Tour& s) = 0; // generate a new successor
	virtual void acceptSuccessor(Tour& s) = 0; // accept the successor.
	virtual void undo(Tour& s) = 0; // Optimisation wise we need to make all successors transformablew
};

class Annealing : public TourSolver
{
public:
	std::vector<int> solve(Graph* g);
	void setSuccessorGenerator(SuccessorGenerator* sg);

	Annealing(CoolingSchedule cs);
	~Annealing();
private:
	SuccessorGenerator* successorGenerator; // how we generate new states.
	CoolingSchedule coolingSchedule;


	// all the random stuff
	std::mt19937 rnd;
	std::uniform_real_distribution<> propabilityDis;
};

class Swapper : public SuccessorGenerator {
public:
	Swapper(Graph* g);

	void createSuccessor(Tour& s);
	void undo(Tour& s);
	void acceptSuccessor(Tour& s);
private:
	int from = 0, to = 0;
	std::uniform_int_distribution<> indiciesDis;
	std::mt19937 rnd;
};

class ReverseSubsection : public SuccessorGenerator {
public:
	ReverseSubsection(Graph* g);
	void createSuccessor(Tour& s);
	void undo(Tour& s);
	void acceptSuccessor(Tour& s);
private:
	int from, to; // 2 iterators that we reveresed inbetween
	std::uniform_int_distribution<> indiciesDis;
	std::mt19937 rnd;
};
#endif