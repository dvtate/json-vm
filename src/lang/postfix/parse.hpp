//
// Created by tate on 16-04-20.
//

#ifndef JSON_LISP_PARSE_HPP
#define JSON_LISP_PARSE_HPP

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "lex.hpp"

class AST {
public:
	enum NodeType {
		STATEMENT,   // marked with semicolon so that it's not used as an operand
		STATEMENT_SERIES,
		END_STATEMENT, // semicolon

		// operand types
		OPERATION = 2, // parsed operator args put in this->members
		EXPRESSION = 3,
		NUM_LITERAL = 4,
		STR_LITERAL = 5,
		ENUM_LITERAL = 6,
		IDENTIFIER = 7,
		MACRO = 8,	// macro literal
		OBJECT = 9,	//
		LIST = 10,

		// containers
		MACRO_OPEN,
		LIST_OPEN,
		PAREN_OPEN,
		CONT_CLOSE,

		// separators
		KV_PAIR,
		COMMA_SERIES,

		// syntax sugar
		MEMBER_REQUEST, // bracket operators
		MACRO_INVOKE,

		BRK_EXPR, // bracket operator
		PAREN_EXPR, // temporary, used to add clarity to macro calls
		OPERATOR,  // un-parsed operator

		DECLARATION,

		INVALID,
	} type;

	Token token;

	std::vector<AST> members;

	// used for reducing constexprs
	// -1 : known volatile
	// 0 : unknown volatility
	// 1 : known static
	signed char volatility : 4;


	AST() = default;
	AST(const NodeType type, const Token token):
		type(type), token(token), members(), volatility(0) {}
	AST(const NodeType type, const Token token, std::vector<AST>&& children):
			type(type), token(token), members(children), volatility(0) {}

};
class SyntaxError {
public:
	Token token;
	std::string msg;

	SyntaxError(const Token& _token, const std::string& _message):
		token(_token), msg(_message) { }
};


AST parse(const std::vector<Token>& tokens);

// convert AST to lisp
std::string debug_AST(const AST& tree);

#endif //JSON_LISP_PARSE_HPP
