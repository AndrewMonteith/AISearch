// AISearch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdinc.h"
#include "Aco.H"
#include "Annealing.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::string formatSize(int size) {
	std::stringstream ss;

	ss << "NEWAISearchfile";
	if (size < 100) {
		ss << "0";
	}
	ss << size << ".txt";

	return ss.str();
}

void writeOutputFile(const std::string& name, Tour& t, int cost) {
	std::ofstream fs("tour" + name + ".txt");

	fs << "NAME = " << name << "," << std::endl;
	fs << "TOURSIZE = " << t.size() << "," << std::endl;
	fs << "LENGTH = " << cost << "," << std::endl;
	
	// Write out elements into fs, separated by commas
	for (auto i = 0; i < t.size(); i++) {
		fs << t[i]+1; // tours 0 indexed in processing, but starts at node 1 in graph.
		if (i < t.size() - 1) {
			fs << ",";
		}
	}

	fs.close();
}

void generateSolutionFiles(std::function<Tour(Graph&)> tourSolverGenerator) {
	const int TourSizes[]{ 12, 17, 21, 26, 42, 58, 175, 180, 535 };
	
	for (int size : TourSizes) {
		auto graph = Graph::LoadFromFile("Tours\\" + formatSize(size));
		
		auto tour = tourSolverGenerator(graph); 
		auto cost = graph.getCostOfTour(tour);
		std::cout << "Solved Tour File " << size << std::endl;
		writeOutputFile(graph.getName(), tour, cost);
	}
}

void findBest535() {
	std::random_device dev;

	std::mt19937 rnd(dev());

	auto startingTempDistribution = std::uniform_real_distribution<>(100'000'000'000, 10'000'000'000'000);
	auto alphaRandomDistribution = std::uniform_real_distribution<>(0.37f, 0.55f);

	auto g = Graph::LoadFromFile("Tours/NEWAISearchfile535.txt");
	auto swapper = ReverseSubsection(&g);

	int bestFoundCost = 1'000'000;

	for (;;) {
		Annealing a(createQuadraticMultiplicativeCS(startingTempDistribution(rnd), alphaRandomDistribution(rnd)));

		a.setSuccessorGenerator(&swapper);

		auto solved = a.solve(&g);
		auto cost = g.getCostOfTour(solved);

		if (cost < bestFoundCost) {
			std::cout << "Found new best cost! " << cost << std::endl;

			writeOutputFile(g.getName(), solved, cost);
			bestFoundCost = cost;
		}
	}
}

int main(int argc, char** argv)
{

	Parameters params{
		0.6, // Evaporation Factor
		3, // Alpha
		6, // Beta
		15, // Q
		0.5, // Default Pheremone,
		25, // Number of iterations
	};

	Parameters params2{
		0.8, // Evaporation Factor
		1, // Alpha
		2, // Beta
		1, // Q
		0.5, // Default Pheremone,
		250, // Number of iterations
	};

	auto acoSolve = [&](Graph& g) {
		auto ant = ACO(g.getNumberOfCities(), params2); //  std::min(g.getNumberOfCities(), 200), params);

		return ant.solve(&g);
	};

	Annealing a(createQuadraticMultiplicativeCS(1000000000000, 0.45)); // Cooling factor, starting temp
	auto annealingSolve = [&](Graph& g) {
		auto swapper = ReverseSubsection(&g);

		a.setSuccessorGenerator(&swapper);

		return a.solve(&g);
	};


	generateSolutionFiles(annealingSolve);
}