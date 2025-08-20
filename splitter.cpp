#include "splitter.h"

#include <vector>
#include <string>
#include <deque>

std::vector<expression>& choose_split_target(std::deque<expression>& stack, const bool as_param) {
	return as_param ?
		stack.front().parameter_children
	  : stack.front().code_block_children;
}

/**
 * Splits the code into a tree of expressions
 * @param source Source code without comments
 * @return Tree of expressions
 */
[[nodiscard]] expression split_by_expressions(const std::string& source) {
	expression out;

	std::deque stack = {expression()};
	expression current;

	bool blank_line = true;
	bool long_string = false;
	bool ready_for_split = false;
	bool add_as_parameter = false;
	bool split_as_parameter = false;
	unsigned int quotes = 0;
	std::string white_spaces;

	for (char c : source + static_cast<char>(1)) {
		if (c == 1) {
			if (ready_for_split) {
				expression front = stack.front();
				stack.pop_front();
				choose_split_target(stack, split_as_parameter).push_back(front);
				ready_for_split = false;
			}
		} else if (c == ' ') {
			if (!blank_line) {
				white_spaces += ' ';
			}
		} else if (c == '\t') {
			if (!blank_line) {
				white_spaces += '\t';
			}
		} else if (((c == '\n' && !blank_line) || c == ';') && !long_string) {
			choose_split_target(stack, add_as_parameter).emplace_back(current);
			current = {};
			blank_line = true;
			white_spaces = "";
		} else if (c == '\n' && !long_string) {
			current = {};
			blank_line = true;
			white_spaces = "";
		} else if (c == '\"') {
			if (ready_for_split) {
				expression front = stack.front();
				stack.pop_front();
				choose_split_target(stack, split_as_parameter).push_back(front);
				ready_for_split = false;
			}
			current.contents += white_spaces;
			white_spaces = "";
			current.contents += '\"';
			blank_line = false;
			if (!long_string) quotes++;
			else quotes--;
			if (quotes >= 3) {
				long_string = true;
			} else if (quotes <= 0) {
				long_string = false;
			}
		} else if (c == '{') {
			stack.emplace_front(current);
			current = {};
			blank_line = true;
			white_spaces = "";
		} else if (c == '}' && stack.size() > 1) {
			if (!current.contents.empty()) choose_split_target(stack, add_as_parameter).emplace_back(current);
			current = {};
			blank_line = true;
			white_spaces = "";
			ready_for_split = true;
			add_as_parameter = false;
		} else if (c == '(') {
			stack.emplace_front(current);
			current = {};
			blank_line = true;
			white_spaces = "";
			add_as_parameter = true;
		} else if (c == ')' && stack.size() > 1) {
			if (!current.contents.empty()) choose_split_target(stack, add_as_parameter).emplace_back(current);
			current = {};
			blank_line = true;
			white_spaces = "";
			ready_for_split = true;
			split_as_parameter = true;
		} else {
			if (ready_for_split) {
				expression front = stack.front();
				stack.pop_front();
				choose_split_target(stack, split_as_parameter).push_back(front);
				ready_for_split = false;
				add_as_parameter = false;
			}
			current.contents += white_spaces;
			white_spaces = "";
			blank_line = false;
			if (long_string) {
				quotes = 3;
			} else {
				quotes = 0;
			}

			current.contents += c;
		}

	}

	out = stack.back();
	return out;
}



[[nodiscard]] std::vector<expression> split_recursive_v2(std::deque<char>& stack_in, expr_separator separator = CODE) {
	std::vector out = {expression()};

	bool string_literal = false;
	bool char_literal = false;
	bool escape = false;
	bool empty_line = true;
	bool had_code = false;
	bool had_parameters = false;
	std::string white_spaces;
	unsigned int quotes_set = 0;
	unsigned int quotes_unset = 0;

	while (!stack_in.empty()) {
		char c = stack_in.front();
		stack_in.pop_front();

		if (string_literal) {
			if (!escape) {
				if (c == '\"') {
					quotes_unset++;
					out.back().contents += c;
					continue;
				}
				if (quotes_unset == quotes_set) {
					quotes_set = 0;
					quotes_unset = 0;
					string_literal = false;
					empty_line = false;
				} else quotes_unset = 0;
				if (c == '\\') escape = true;
			}
		}
		if (char_literal) {
			if (!escape) {
				if (c == '\'') char_literal = false;
				if (c == '\\') escape = true;
			}
			out.back().contents += c;
		} else if (c == '\"') {
			quotes_set++;
		} else {
			if (quotes_set == 1 || quotes_set == 3) {
				quotes_unset = 0;
				string_literal = true;
			} else quotes_set = 0;
		}

		if (c == '\'') {
			char_literal = true;
		}

		if (c == '{') {
			white_spaces = "";
			empty_line = true;
			std::vector<expression> first = split_recursive_v2(stack_in, CODE);
			out.back().code_block_children.insert(
				out.back().code_block_children.end(),
				first.begin(), first.end()
				);
			if (
				out.back().code_block_children.back().contents.empty() &&
				out.back().code_block_children.back().code_block_children.empty() &&
				out.back().code_block_children.back().parameter_children.empty()
			) {
				out.back().code_block_children.pop_back();
			}
			if (out.back().code_block_children.empty()) out.back().had_code = true;
			had_code = true;
		} else if (c == '(') {
			white_spaces = "";
			empty_line = true;
			std::vector<expression> first = split_recursive_v2(stack_in, COMMA);
			out.back().parameter_children.insert(
				out.back().parameter_children.end(),
				first.begin(), first.end()
				);
			if (
				out.back().parameter_children.back().contents.empty() &&
				out.back().parameter_children.back().code_block_children.empty() &&
				out.back().parameter_children.back().parameter_children.empty()
			) {
				out.back().parameter_children.pop_back();
			}
			if (out.back().parameter_children.empty()) out.back().had_parameters = true;
			had_parameters = true;
		} else if (c == '}' || c == ')') {
			return out;
		} else if (
			(((c == '\n' || c == ';') && separator == CODE) ||
			 (c == ',' && separator == COMMA)) &&
			!(
				out.back().contents.empty() &&
				out.back().parameter_children.empty() &&
				out.back().code_block_children.empty())
		) {
			out.emplace_back();
			had_code = false;
			had_parameters = false;
			empty_line = true;
			white_spaces = "";
		} else if (c == ' ' || c == '\t' || c == '\n') {
			if (!empty_line) {
				if (true) white_spaces += "_";
				else if (c == ' ') white_spaces += " ";
				else if (c == '\t') white_spaces += "\\t";
				else white_spaces += "\\n";
			}
		} else {
			if (had_code || had_parameters) {
				out.emplace_back();
				had_code = false;
				had_parameters = false;
				empty_line = true;
				white_spaces = "";
			}
			empty_line = false;
			out.back().contents += white_spaces;
			out.back().contents += c;
			white_spaces = "";
		}
	}

	return out;
}

[[nodiscard]] expression split_recursive_v2(const std::string& source) {
	std::deque<char> stack{source.begin(), source.end()};
	return {split_recursive_v2(stack)};
}