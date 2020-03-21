//
// Created by tate on 25-02-20.
//

#ifndef JSON_LISP_TOKENIZER_HPP
#define JSON_LISP_TOKENIZER_HPP

#include <string>

struct Token {
	enum t {
		OPERATOR,
		STRING,
		PAREN,
		BRACE,
		BRACKET,
		COMMA,
		COLON,
		NUM,
		DOT,
		END,
		ERR,
	} type;
	std::string token;
};

typedef struct Token Token;

Token tokenizer(const std::string& buff, size_t& offset);

#endif //JSON_LISP_TOKENIZER_HPP
