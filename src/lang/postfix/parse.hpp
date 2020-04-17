//
// Created by tate on 16-04-20.
//

#ifndef JSON_LISP_PARSE_HPP
#define JSON_LISP_PARSE_HPP

#include "lex.hpp"

struct AST {
	std::vector<AST> members;
	enum NodeType {
		// expression ending in semicolon
		STATEMENT,
		//
		EXPRESSION,
		
		LITERAL,
		MACRO,
		FILE,
		IDENTIFIER,
		OBJECT,
		KV_PAIR,
		LIST,
		MEMBER_REQUEST,
		MACRO_INVOKE,

	} type;
	Token token;

	// used for reducing constexprs
	// -1 : known volatile
	// 0 : unknown volatility
	// 1 : known static
	const signed char volatility;

};

struct AST parse(const std::vector<struct Token>& tokens);


#endif //JSON_LISP_PARSE_HPP
