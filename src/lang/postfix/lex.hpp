//
// Created by tate on 25-02-20.
//

#ifndef JSON_LISP_LEX_HPP
#define JSON_LISP_LEX_HPP

#include <string>
#include <vector>


class Token {
public:
	enum t {
		OPERATOR,
		IDENTIFIER,
		STRING,
		NUMBER,
		END,
		ERROR,
		EMPTY,
	} type;
	std::string token;

	// character index of the token in the scanned file
	unsigned long long int pos;


	Token(const t _type, std::string&& _token):
		type(_type), token(_token), pos(0) {}

	Token() = default;
};
typedef struct Token Token;

std::vector<struct Token> tokenize_stream(std::istream& in);


#endif //JSON_LISP_LEX_HPP
