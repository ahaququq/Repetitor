#include <fstream>
#include <iostream>

#include "preprocessor.h"
#include "splitter.h"

void print_expressions(
	const expression& x,
	int depth,
	std::ostream &out = std::cout,
	unsigned int skip_depth = 0
);

void print_expressions(
	const std::vector<expression>& x,
	const int depth,
	std::ostream &out = std::cout,
	const unsigned int skip_depth = 0
) {
	for (const expression &expr: x) {
		print_expressions(expr, depth, out, skip_depth);
	}
}

void print_expressions(
	const expression& x,
	const int depth,
	std::ostream &out,
	const unsigned int skip_depth
) {
	if (skip_depth == 0) {
		out << std::string(depth, '\t') << "`" << x.contents << "`";
		if (!x.parameter_children.empty()) out << " (\n";
	}
	print_expressions(x.parameter_children, depth + 1, out, skip_depth > 0 ? skip_depth - 1 : 0);
	if (skip_depth == 0) {
		if (x.parameter_children.empty() && x.code_block_children.empty()) {
			out << ";\n";
		} else if (x.parameter_children.empty()) {
			out << " {\n";
		} else if (x.code_block_children.empty()) {
			out << std::string(depth, '\t') << ");\n";
		} else {
			out << std::string(depth, '\t') << ") {\n";
		}
	}
	print_expressions(x.code_block_children, depth + 1, out, skip_depth > 0 ? skip_depth - 1 : 0);
	// for (const expression &expr: x.code_block_children) {
	// 	print_expressions(expr, depth + 1, out, skip_depth > 0 ? skip_depth - 1 : 0);
	// }
	if (skip_depth == 0 && !x.code_block_children.empty()) {
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

	expression expressions = split_recursive_v2(processed);

	out << "-----\n";
	print_expressions(expressions, -1, out, 1);


	return 0;
}
