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
		fs << t[i];
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

int main(int argc, char** argv)
{
	auto acoSolve = [](Graph& g) {
		auto ant = ACO(std::min(g.getNumberOfCities(), 200));

		return ant.solve(&g);
	};

	Annealing a(0.001, 10000); // Cooling factor, starting temp
	auto annealingSolve = [&](Graph& g) {
		auto swapper = Swapper(&g);

		a.setSuccessorGenerator(&swapper);

		return a.solve(&g);
	};

	 generateSolutionFiles(acoSolve);
}