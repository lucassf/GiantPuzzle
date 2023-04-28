#include <fstream>
#include <vector>
#include <sstream>


std::vector<std::vector<int>> readCsvData(const std::string& filename) {
	std::vector<std::vector<int>> data;
	std::string line;

	std::fstream file(filename, std::ios::in);

	if (file.is_open())
		while (std::getline(file, line)) {
			std::vector<int> row;
			std::stringstream ss(line);
			std::string cell = "";

			while (std::getline(ss, cell, ',')) {
				row.push_back(std::stoi(cell));
			}
			data.push_back(row);
		}
	return data;
}
