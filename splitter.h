#pragma once
#include <string>
#include <vector>

struct expression {
	std::vector<expression> code_block_children; // else { code; more code }
	std::vector<expression> parameter_children;  // print("hello world", 1)
	std::string contents;
	bool had_parameters = false;
	bool had_code = false;
};

[[nodiscard]] expression split_by_expressions(const std::string& source);

[[nodiscard]] expression split_recursive_v2(const std::string& source);

enum expr_separator {
	CODE,	// `\n` & `;`
	COMMA,	// `,`
};
