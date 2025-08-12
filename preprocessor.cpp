#include <string>

#include "preprocessor.h"


std::string remove_comments(const std::string& source) {
	std::string output;

	bool line_comment	= false;
	bool string_literal	= false;
	bool char_literal	= false;
	bool escape			= false;
	bool first_slash	= false;
	bool first_star		= false;
	bool blank_line		= true;
	bool block_comment_only = false;
	unsigned int literal_depth			= 0;
	unsigned int last_double_quotes		= 0;
	unsigned int muted_indent_spaces	= 0;
	unsigned int muted_indent_tabs		= 0;
	unsigned int block_comment_depth	= 0;
	for (char c: source) {
		if (block_comment_depth) {
			if (c == '/') {
				first_slash = true;
				if (first_star) {
					block_comment_depth--;
					first_star = false;
					first_slash = false;
				}
			} else if (c == '*') {
				first_star = true;
				if (first_slash) {
					block_comment_depth++;
					first_slash = false;
					first_star = false;
				}
			} else {
				first_slash = false;
				first_star = false;
			}
		} else if (line_comment) {
			first_slash = false;
			if (c == '\n') {
				line_comment = false;
				if (!blank_line) {
					blank_line = true;
					output += '\n';
				} else {
					muted_indent_tabs = 0;
					muted_indent_spaces = 0;
				}
			}
		} else if (escape) {
			output += "\\"; output += c; escape = false;
		} else if (string_literal) {
			if (c == '\"') {
				last_double_quotes++;
				if (last_double_quotes >= literal_depth) {
					string_literal = false;
					last_double_quotes = 0;
				}
				output += c;
			} else if (c == '\\') {
				escape = true;
			} else {
				last_double_quotes = 0;
				output += c;
			}
		} else if (char_literal) {
			if (c == '\'') {
				char_literal = false;
				output += c;
			} else if (c == '\\') {
				escape = true;
			} else {
				output += c;
			}
		} else {
			if (c != '\"' && (last_double_quotes == 1 || last_double_quotes == 3)) {
				string_literal = true;
				literal_depth = last_double_quotes;
				output += c;
			} else if (c == '\\') {
				escape = true;
			} else {
				if (c == '\"') {
					last_double_quotes++;
				} else {
					last_double_quotes = 0;
				}
				if (c == '\'') {
					char_literal = true;
					output += '\'';
				} else if (first_slash && (c == '/' || c == '*')) {
					if (c == '/') {
						line_comment = true;
					} else {
						block_comment_depth++;
						muted_indent_tabs = 0;
						muted_indent_spaces = 0;
						if (blank_line) block_comment_only = true;
					}
				} else {
					if (first_slash) {
						output += '/';
						first_slash = false;
					}
					if (c == '/') {
						first_slash = true;
					} else if (c == ' ') {
						if (!blank_line) output += '_';
						else muted_indent_spaces++;
					} else if (c == '\t') {
						if (!blank_line) output += "~~~~";
						else muted_indent_tabs++;
					} else if (c == '\n') {
						if (!block_comment_only)
							output += '\n';
						blank_line = true;
						block_comment_only = false;
					} else {
						block_comment_only = false;
						if (blank_line) {
							output += std::string(muted_indent_tabs, '\t');
							output += std::string(muted_indent_spaces, ' ');
							muted_indent_tabs = 0;
							muted_indent_spaces = 0;
						}
						output += c;
						blank_line = false;
					}
				}
			}
		}
	}
	return output;
}
