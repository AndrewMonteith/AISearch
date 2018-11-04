// AISearch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdinc.h"
#include "Aco.H"
#include "Annealing.h"

int main(int argc, char** argv)
{
	Graph g = Graph::LoadFromFile("Tour012.txt"); //
	/*
	ACO a(180);  

	Timer t;

	auto tour = a.solve(&g);

	auto elasped = t.elapsed();

	std::cout << "Took:" << elasped << " to execute." << std::endl;

	for (int i : tour) {
		std::cout << i << ",";
	}
	std::cout << std::endl;
	*/
	
	Annealing ann(0.0001, 10000);
	Swapper swap(&g);
	ReverseSubsection rs(&g);

	ann.setSuccessorGenerator(&rs);

	for (auto i = 0; i < 100; i++) {
		auto tour = ann.solve(&g);

		std::cout << "Cost of Tour " << g.getCostOfTour(tour) << std::endl;
	}
	
//	for (int i : tour) {
//		std::cout << i << ",";
//	}
//	std::cout << std::endl;
}