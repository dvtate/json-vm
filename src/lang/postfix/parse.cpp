//
// Created by tate on 16-04-20.
//

#include "parse.hpp"

// using a shift-reduce parser

std::vector<std::string> keywords;
std::unordered_map<std::string, short> op_prec = {
		{ }
		{ "+", 14 },
		{ "-", 14 },
		{ }
};

struct AST parse(const std::vector<struct Token>& tokens) {
	std::vector<struct AST> expr_stack;
	std::vector<


	// if multi-char operator sequence try to reduce
	// always reduce containers
	//
}