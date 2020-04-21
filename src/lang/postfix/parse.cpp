//
// Created by tate on 16-04-20.
//

#include "parse.hpp"

// using a shift-reduce parser
std::vector<std::string> keywords;

// modified from https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence
std::unordered_map<std::string, signed char> op_prec = {
		{ "{", 22  },
		{ "(",  21 },
		{ "[",  20 },
		{ ".",	20 },
		{ "!", 17 },
		{ " neg", 17 },
		{ "**", 16 },
		{ "*", 	15 },
		{ "/", 	15 },
		{ "%", 	15 },
		{ "+", 	14 },
		{ "-", 	14 },
//		{ "<<", 13 },
//		{ ">>", 13 },
//		{ ">>>", 13 },
		{ "<", 	12 },
		{ ">", 	12 },
//		{ "<=", 12 },
//		{ ">=", 12 },
		{ "==", 11 },
//		{ "!=", 11 },
		{ "&", 	10 },
		{ "^", 	9 },
		{ "|", 	8 },
		{ "&&", 6 },
		{ "||", 5 },
//		{ "?", 	4 }, // tern... if ever added..
		{ "=", 	3 }, // assignment
		{ ":", 	2 }, // key value pair
		{ ",", 	1 }, // comma seq
		{ "let", 0 },
		{ "]",  0 },
		{ ";", 	0 }, // statement separator
		{ ")", 0 },
};

std::vector<std::string> kw_literals = {
		"null", "empty", "true", "false",
};

static inline AST next_node(const std::vector<Token>& tokens, size_t& i, std::vector<AST> stack) {

	Token t = tokens[i];

	if (t.type == Token::t::ERROR) {
		throw SyntaxError(t, t.token);
	}

	// recombine multi-char operators
	if (t.type == Token::t::OPERATOR && tokens.size() - i > 1 && tokens[i + 1].type == Token::t::OPERATOR) {
		const std::string combined = tokens[i].token + tokens[i + 1].token;
		static const std::vector<std::string> mc_ops = { "&&", "||", "<=", ">=", "==", "!=", "**" };

		// special case for macro-open
		if (combined == "(:") {
			t.token = combined;
			const struct AST ret = { .type = AST::NodeType::CONT_OPEN , .token = t };
			i += 2;
			std::cout <<"parse:shift:macro open\n";
			return ret;

		} else if (std::find(mc_ops.begin(), mc_ops.end(), combined) != mc_ops.end()) {
			t.token = combined;
			i += 2;
			std::cout <<"parse:shift:multichar op: " <<combined <<std::endl;
			return (struct AST) { .type = AST::NodeType::OPERATOR, .token = t });

		}
	}
	if (t.type == Token::t::NUMBER) {
		i++;
		return { .type = AST::NodeType::NUM_LITERAL, .token = t };
	};
	if (t.type == Token::t::STRING) {
		i++;
		return { .type = AST::NodeType::STR_LITERAL, .token = t };
	};
	if (t.type == Token::t::IDENTIFIER) {

	}


	if (t.type == Token::t::OPERATOR) {
		if (t.token[0] == '[' || t.token[0] == '(') {
			const AST ret = { .type = AST::NodeType::CONT_OPEN, .token = t };
			i++;
			return ret;
		}

		// unary minus
		// minus preceded by another operator
		if (stack[l - 1].token.type  == Token::t::OPERATOR &&
			stack[l - 1].token.token == "-" &&
			stack[l - 2].token.type  == Token::t::OPERATOR &&
			last.token.token != "-") {
			last.token.token = " neg";
		}
	}


}


static inline size_t prev_operator(std::vector<struct AST>& stack) {
	ssize_t ret = stack.size();
	while (--ret >= 0) {
		if (stack[ret].type == AST::NodeType::OPERATOR) {

		}
	}
}

static inline bool reduce_operators(std::vector<struct AST>& stack, const Token& t) {

	// for each operator
	//    if next operator has higher precedence, go to next operator
	//	  if next operator has lower or equal precedence, do reduce
	//

	stack.push_back(t);
	size_t op = next_operator(stack, 0);
	size_t nop = next_operator(stack, op);
	try {
		while (nop < stack.size() && op < stack.size()) {
			if (op_prec.at(stack[op].token.token) >= op_prec.at(stack[nop].token.token)) {

				return true;
			} else {
				op = nop;
				nop = next_operator(stack, op);
			}
		}

	} catch (...) {
		std::cerr <<"reduce_operators errr";
	}

	if (stack.back().pos = t.pos) {
		stack.pop_back();
	}
}


static inline bool reduce(const std::vector<struct Token>& tokens, size_t& i, std::vector<struct AST>& stack) {
	// lookahead token
	Token t = tokens[i];
	const AST last = stack[stack.size() - 1];
	const l = stack.size() - 1;

	if (stack[l - 1].token.token == "!" && stack)

	// unary not

	// try to reduce containers
	for (int i = stack)
}

struct AST parse(const std::vector<struct Token>& tokens) {
	std::vector<struct AST> expr_stack;

	size_t i = 0;


	while (i < tokens.size()) {
		AST tok = next_node(tokens, i, expr_stack);

		////////// REDUCE:
	}


	// if multi-char operator sequence try to reduce
	// always reduce containers
	//
}
