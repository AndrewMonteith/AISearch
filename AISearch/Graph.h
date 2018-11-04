#pragma once

#include "stdinc.h"

class Graph
{
public:
	Graph(std::string name, int numberOfCities, std::vector<int> weights);
	~Graph();

	static Graph LoadFromFile(const std::string fileName);

	int getWeight(int, int) const;
	const std::string& getName() const;
	int getNumberOfCities();

	int getCostOfTour(const std::vector<int>& tour);

private:
	std::vector<int> weights;
	std::string name;
	int numberOfCities;
};
