#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

template <typename T>
class DecisionTable {
private:
	int rowCount = 0;
	int colCount = 0;

	void addRow (std::string& line);
	void calcColumns (std::string& line);

	// convert
	template<typename T, typename V>
	V convert (T input) {
		return static_cast<V>(input);
	}

	template<>
	int convert<std::string, int> (std::string input) {
		return std::stoi(input);
	}

public:
	std::vector<std::vector<T>*> table{};
	DecisionTable ();
	~DecisionTable ();

	void addColumn ();
	void readData (std::string path);
	std::vector<int> getDecision (std::vector<int>& data);
};

template<typename T>
DecisionTable<T>::DecisionTable() {
	std::vector<int>* decision = new std::vector<int>;
	decision->resize(rowCount);
	table.insert(table.end(), decision);
	colCount++;
}

template<typename T>
DecisionTable<T>::~DecisionTable() {
	for (auto a : table) {
		delete(a);
	}
}

template<typename T>
void DecisionTable<T>::addColumn() {
	std::vector<T>* column = new std::vector<T>;
	column->resize(rowCount);
	table.insert(table.end() - 1, column);
}

template<typename T>
void DecisionTable<T>::readData(std::string path) {
	std::ifstream file(path);
	if (&file == nullptr) {
		std::cout << "Invalid file path" << std::endl;
	}
	std::string line;
	std::streampos old = file.tellg();
	std::getline(file, line);
	calcColumns(line);
	file.seekg(old);

	while (std::getline(file, line)) {
		addRow(line);
	}
}

template<typename T>
void DecisionTable<T>::addRow(std::string& line) {
	std::string delimiter = ", ";
	std::string token;
	int index = 0;

	// iterate through the columns
	for (int a = 0; a <  table.size()-1; a++) {
		// dont go out of array bounds
		if (index > (table.size() - 1)) {
			std::cout << "Invalid size" << std::endl;
			break;
		}
		// get the next token. if the token is empty insert a null.
		token = line.substr(0, line.find(delimiter));
		if (token.empty()) {
			table[index]->insert(table[index]->end(), NULL);
			index++;
			continue;
		}
		line.erase(0, line.find(delimiter) + delimiter.length());
		table[index]->insert(table[index]->end(), convert<std::string, T>(token));
		index++;
	}
	// increment the answer column from previous answer column value.
	table[table.size() - 1]->insert(table[table.size() - 1]->end(), rowCount);
	rowCount++;
}

template<typename T>
void DecisionTable<T>::calcColumns(std::string& line) {
	std::string delimiter = ", ";
	std::string token;
	// until line is empty, extract tokens and keep a count to determine column count
	while (!line.empty()) {
		token = line.substr(0, line.find(delimiter));
		line.erase(0, line.find(delimiter) + delimiter.length());
		if (!token.empty()) {
			addColumn();
			colCount++;
		}
	}
}

template<typename T>
std::vector<int> DecisionTable<T>::getDecision(std::vector<int>& data) {
	std::vector<int> result{};
	int resultIndex = 0;
	int index = 0;
	bool addToResult;
	for (int a = 0; a < rowCount; a++) {
		addToResult = true;
		index = 0;
		for (int b = 0; b < table.size()-2; b++) {
			if (table[b]->at(a) == 0) {
				continue;
			}
			if (table[b]->at(a) != data.at(index)) {
				addToResult = false;
				break;
			}
			index++;
		}
		if (addToResult) {
			result.insert(result.end(), table[table.size()-1]->at(resultIndex));
		}
		resultIndex++;
	}
	// remove any conflicts here
	
	//
	return result;
}
