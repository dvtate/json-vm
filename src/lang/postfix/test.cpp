#include <iostream>

#include "tokenizer.hpp"

int main() {
	for (;;) {
		size_t i = 0;
		std::string inp;
		std::getline(std::cin, inp);
		Token tok;
		do {
			tok = tokenizer(inp, i);
			std::cout <<'\t' <<tok.type <<':' <<tok.token <<std::endl;
		} while (tok.type != Token::t::END);
	}
}