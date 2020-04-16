#include <iostream>
#include <sstream>
#include "tokenizer.hpp"

int main() {
	for (;;) {
		std::string inp;
		std::getline(std::cin, inp);
		std::stringstream ss(inp);
		std::vector<Token> toks = tokenize_stream(ss);
		for (Token tok : toks) {
			std::cout <<'\t' <<tok.type <<':' <<tok.token <<std::endl;
		}
	}
}