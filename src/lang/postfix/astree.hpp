//
// Created by tate on 25-02-20.
//

#ifndef JSON_LISP_ASTREE_HPP
#define JSON_LISP_ASTREE_HPP

#include <string>
#include <vector>
#include <iostream>

#include <unordered_map>

#include "tokenizer.hpp"

class SyntaxTree {
public:
	class Node {
	public:
		enum token_type_t { JSON, OPERATOR, BLOCK } type;
		std::string token;
		std::vector<size_t> branches;

		Node(std::string): {

		}
	};

	std::vector<Node> tokens;

	SyntaxTree(){}


};



#endif //JSON_LISP_ASTREE_HPP
