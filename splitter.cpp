#include "splitter.h"

#include <vector>
#include <string>
#include <deque>

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
	unsigned int quotes = 0;
	std::string white_spaces;

	for (char c: source) {
		if (c == ' ') {
			if (!blank_line) {
				white_spaces += ' ';
			}
		} else if (c == '\t') {
			if (!blank_line) {
				white_spaces += '\t';
			}
		} else if (((c == '\n' && !blank_line) || c == ';') && !long_string) {
			stack.front().children.emplace_back(current);
			current = {};
			blank_line = true;
			white_spaces = "";
		} else if (c == '\n' && !long_string) {
			current = {};
			blank_line = true;
			white_spaces = "";
		} else if (c == '\"') {
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
			if (!current.contents.empty()) stack.front().children.emplace_back(current);
			current = {};
			blank_line = true;
			white_spaces = "";
			expression front = stack.front();
			stack.pop_front();
			stack.front().children.push_back(front);
		} else {
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
