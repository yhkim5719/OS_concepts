#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<int> myint;

int main (int argc, char** argv) {
	std::fstream file;
	std::string word;

	file.open(argv[1]);

	while (file >> word) {
		myint.push_back(stoi(word));
	}

	for (int i = 0; i < myint.size(); i++) {
		std::cout << myint[i] << '\n';
	}

	return 0;
}
