//
// Created by tate on 25-02-20.
//

#ifndef JSON_LISP_TOKENIZER_HPP
#define JSON_LISP_TOKENIZER_HPP

#include <string>
#include <vector>

struct Token {
	enum t {
		OPERATOR,
		IDENTIFIER,
		STRING,
		NUMBER,
		END,
		ERROR,
	} type;
	std::string token;
};

typedef struct Token Token;


std::vector<struct Token> tokenize_stream(std::istream& in);


#endif //JSON_LISP_TOKENIZER_HPP
