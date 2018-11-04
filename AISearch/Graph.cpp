#include "Graph.h"

#include <fstream>
#include <ctype.h>
#include <regex>

typedef std::string::const_iterator str_iter;
typedef std::function<bool(char)> char_predicate;

inline int flattenCity(int c1, int c2, int numberOfCities) {
	return (c1 - 1)*numberOfCities + (c2 - 2);
}

Graph::Graph(std::string name, int numberOfCities, std::vector<int> weights) 
	:name(name), numberOfCities(numberOfCities), weights(weights) {}

const std::string& Graph::getName() const {
	return this->name;
}

int Graph::getNumberOfCities() {
	return numberOfCities;
}

// Distance Between City c1 and c2
int Graph::getWeight(int c1, int c2) const {
	return c1==c2 ? 0 : weights[flattenCity(c1, c2, numberOfCities)];
}

int Graph::getCostOfTour(const std::vector<int>& tour) {
	auto sum = 0;
	for (auto i = 0; i < numberOfCities-1; i++) {
		sum += getWeight(tour[i], tour[i + 1]);
	}
	sum += getWeight(tour[0], tour.back()); 

	return sum;
}

Graph::~Graph()
{
}

std::string readWhilst(str_iter& it, str_iter end, char_predicate acceptChar, char_predicate stopReading) {
	std::string buffer;

	for (; it != end && !stopReading(*it); it++) {
		if (acceptChar(*it))
			buffer += *it;
	}

	if (it != end)
		it++; // Skip over the character that caused us to stop reading.
	
	return buffer;
}

void skipWhitespace(str_iter& it, str_iter end) {
	for (; it != end && isspace(*it); it++);
}

std::string readUntilComma(str_iter& it, str_iter end, char_predicate acceptChar) {
	return readWhilst(it, end, acceptChar, [](char c) { return c == ','; });
}

std::string readDelimiteredItem(str_iter& it, str_iter end, char_predicate acceptChar) {
	skipWhitespace(it, end);

	return readUntilComma(it, end, acceptChar);
}

std::string getMatch(const std::string& contents, const char* pattern) {
	std::regex reg(pattern);
	std::smatch baseMatch;

	std::regex_search(contents, baseMatch, reg);
		
	return baseMatch[1].str();
}

Graph Graph::LoadFromFile(const std::string fileName) {
	// Data can be dirty, meaning there can be unexpected characters within the text. 

	std::ifstream f(fileName);

	if (f) {
		const std::string contents((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());

		str_iter it = contents.begin(), end = contents.end();
		
		
		auto nameLine = readDelimiteredItem(it, end, [](char c) { return true; });
		auto name = getMatch(nameLine, "=\\s*(.+)");
		
		auto numberOfCities = std::stoi(readDelimiteredItem(it, end, isdigit));

		// The distances are symmetric hence the distances are given as 
		// distance between 7 cities, 6, 5, 4, 3, 2, 1

		// Going to store it in one big contiguos memory block
		std::vector<int> cityDistances = std::vector<int>(numberOfCities*numberOfCities);

		for (auto i = 1; i <= numberOfCities; i++) {
			for (auto j = i + 1; j <= numberOfCities; j++) {
				auto dist = std::stoi(readDelimiteredItem(it, end, isdigit));

				cityDistances[flattenCity(i, j, numberOfCities)] = dist;
				cityDistances[flattenCity(j, i, numberOfCities)] = dist;
			}
		}

		return Graph(name, numberOfCities, cityDistances);
	}
	else {
		throw std::runtime_error("couldn't find the file!");
	}


}