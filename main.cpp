#include <fstream>
#include <iostream>

#include "preprocessor.h"

int main() {
	std::fstream in("./main.src");
	std::fstream out("./main.out");

	std::string sourceCode;

	std::string line;
	getline(in, line);
	while (in) {
		sourceCode += line + "\n";
		getline(in, line);
	}

	std::string processed = remove_comments(sourceCode);
	std::cout << processed;
	out		  << processed;

	return 0;
}
