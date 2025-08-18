#pragma once
#include <string>
#include <vector>

struct expression {
	std::vector<expression> children;
	std::string contents;
};

[[nodiscard]] expression split_by_expressions(const std::string& source);
