#include <fstream>
#include <iostream>

#include "preprocessor.h"
#include "splitter.h"

void print_expressions(
	const expression& x,
	const int depth,
	std::ostream &out = std::cout,
	const unsigned int skip_depth = 0
) {
	if (skip_depth == 0) {
		out << std::string(depth, '\t') << x.contents;
		if (!x.children.empty()) out << " {";
		else out << ";";
		out << "\n";
	}
	for (const expression &expr: x.children) {
		print_expressions(expr, depth + 1, out, skip_depth > 0 ? skip_depth - 1 : 0);
	}
	if (skip_depth == 0 && !x.children.empty()) {
		out << std::string(depth, '\t') << "}\n";
	}
}

int main() {
	std::ifstream in("./main.txt");
	std::ofstream out("./main.out");

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

	expression expressions = split_by_expressions(processed);

	out << "-----\n";
	print_expressions(expressions, -1, out, 1);


	return 0;
}
