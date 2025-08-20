#include <fstream>
#include <iostream>

#include "preprocessor.h"
#include "splitter.h"

void print_expressions(
	const expression& x,
	int depth,
	std::ostream &out,
	unsigned int skip_depth,
	expr_separator separator, bool last = false
);

void print_expressions(
	const std::vector<expression>& x,
	const int depth,
	std::ostream &out,
	const unsigned int skip_depth,
	expr_separator separator
) {
	unsigned long i = x.size();
	for (const expression &expr: x) {
		print_expressions(expr, depth, out, skip_depth, separator, !--i);
	}
}

void print_expressions(
	const expression& x,
	const int depth,
	std::ostream &out,
	const unsigned int skip_depth,
	expr_separator separator,
	bool last
) {
	if (skip_depth == 0) {
		if (separator == CODE) out << std::string(depth, '\t');
		out << "`" << x.contents << "`";
		if (x.had_parameters) out << " ()";
		if (x.had_code) out << " {}";
		if (!x.parameter_children.empty()) out << " (";
		out << std::flush;
	}
	print_expressions(x.parameter_children, 0, out, skip_depth > 0 ? skip_depth - 1 : 0, COMMA);
	out << std::flush;
	if (skip_depth == 0) {
		if (x.parameter_children.empty() && x.code_block_children.empty()) {
			if (separator == CODE) out << ";\n";
			else if (!last) out << ", ";
		} else if (x.parameter_children.empty()) {
			out << " {\n";
		} else if (x.code_block_children.empty()) {
			if (separator == CODE) out << ");\n";
			else {
				out << ")";
				if (!last) out << ", ";
			}
		} else {
			out << ") {\n";
		}
		out << std::flush;
	}
	print_expressions(x.code_block_children, depth + 1, out, skip_depth > 0 ? skip_depth - 1 : 0, CODE);
	out << std::flush;
	if (skip_depth == 0 && !x.code_block_children.empty()) {
		out << std::string(depth, '\t') << "}\n";
		out << std::flush;
	}
}

int main() {
	std::ifstream in("./run/main.txt");
	std::ofstream out("./run/main.out");

	std::string sourceCode;

	std::string line;
	getline(in, line);
	while (in) {
		sourceCode += line + "\n";
		getline(in, line);
	}

	std::string processed = remove_comments(sourceCode);
	//std::cout << processed;
	out		  << processed;

	expression expressions = split_recursive_v2(processed);

	out << "-----\n";
	print_expressions(expressions, -1, out, 1, CODE);


	return 0;
}
