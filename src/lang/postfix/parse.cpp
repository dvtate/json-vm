//
// Created by tate on 16-04-20.
//

#include "parse.hpp"


// convert AST to lisp
/// TODO: indentation + depth tracking
std::string debug_AST(const AST& tree) {
	std::string ret = "(";
	ret += tree.token.token;
	ret +=(int) tree.type;
	for (const AST& m : tree.members)
		ret += " " + debug_AST(m);
	ret += ')';
	return ret;
}

// using a shift-reduce parser
std::vector<std::string> keywords;

// modified from https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence
std::unordered_map<std::string, signed char> op_prec = {
		{ "{", 22  },
		{ "(",  21 },
		{ "[",  20 },
		{ ".",	20 },
		{ "!", 17 },
		{ "neg", 17 },
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
		{ "eof", 0 }
};



std::vector<std::string> kw_literals = {
		"null", "empty", "true", "false",
};


static inline bool isOperand(const AST& n) {
	return n.type > AST::NodeType::OPERATION && n.type < AST::NodeType::LIST;
}

static inline AST next_node(const std::vector<Token>& tokens, size_t& i, std::vector<AST> stack) {

	Token t = tokens[i];

	if (t.type == Token::t::ERROR) {
		throw std::vector<SyntaxError>{SyntaxError(t, t.token)};
	}

	// recombine multi-char operators
	if (t.type == Token::t::OPERATOR && tokens.size() - i > 1 && tokens[i + 1].type == Token::t::OPERATOR) {
		const std::string combined = tokens[i].token + tokens[i + 1].token;
		static const std::vector<std::string> mc_ops = { "&&", "||", "<=", ">=", "==", "!=", "**" };

		// special case for macro-open
		if (combined == "(:") {
			t.token = combined;
			const AST ret = AST(AST::NodeType::MACRO_OPEN, t);
			i += 2;
			std::cout <<"parse:shift:macro open\n";
			return ret;

		} else if (std::find(mc_ops.begin(), mc_ops.end(), combined) != mc_ops.end()) {
			t.token = combined;
			i += 2;
			std::cout <<"parse:shift:multichar op: " <<combined <<std::endl;
			return AST(AST::NodeType::OPERATOR, t );
		}
	}
	if (t.type == Token::t::NUMBER) {
		i++;
		return AST(AST::NodeType::NUM_LITERAL, t);
	}
	if (t.type == Token::t::STRING) {
		i++;
		return AST(AST::NodeType::STR_LITERAL, t);
	}
	if (t.type == Token::t::IDENTIFIER) {
		i++;
		return AST(AST::NodeType::IDENTIFIER, t);
	}


	if (t.type == Token::t::OPERATOR) {
		// container openings
		if (t.token[0] == '(') {
			i++;
			return AST(AST::NodeType::PAREN_OPEN, t);
		}
		if (t.token[0] == '[') {
			i++;
			return AST(AST::NodeType::LIST_OPEN, t);
		}
		if (t.token[0] == ')' || t.token[0] == ']') {
			i++;
			return AST(AST::NodeType::CONT_CLOSE, t);
		}

		// unary minus (negation)
		if (t.token == "-" &&
				stack.back().token.type == Token::t::OPERATOR) {
			i++;
			t.token == "neg";
			return AST(AST::NodeType::OPERATOR, t);
		}

		if (t.token == "eof") {
			i++;
			return AST(AST::NodeType::INVALID, t);
		}

		i++;
		return AST(AST::NodeType::OPERATOR, t);
	}

	throw SyntaxError(t, "Unknown token");
}

static inline bool has_multi_prefix_kw(std::vector<AST>& stack) {
	for (const AST& n : stack)
		if (n.token.type == Token::t::IDENTIFIER)
			if (n.token.token == "if" || n.token.token == "while")
				return true;
	return false;
}
static inline int prev_operator(std::vector<AST>& stack) {
	ssize_t ret = stack.size();
	while (--ret >= 0)
		if (stack[ret].type == AST::NodeType::OPERATOR)
			return ret;

	return -1;
}

static inline void reduce_operator(std::vector<AST>& stack, const size_t i) {
	// all operators are binary except: "neg", "let", and !
	// oddballs: , ; :
	AST n = stack[i];
	const std::string op = n.token.token;

	// return kv pair
	if (op == ":") {
		if (stack.size() - i != 2 || stack.size() < 3)
			throw std::vector<SyntaxError>{SyntaxError(n.token, "invalid colon")};

		const AST r = stack.back();
		stack.pop_back();
		stack.pop_back();
		const AST l = stack.back();
		n.type = AST::NodeType::KV_PAIR;
		n.members.emplace_back(l);
		n.members.emplace_back(r);
		stack.back() = n;
		return;
	}

	// unary
	if (op == "neg" || op == "!") {
		if (stack.size() - i != 2)
			throw std::vector<SyntaxError>{SyntaxError(n.token, "invalid unary operation")};
		n.type = AST::OPERATION;
		n.members.emplace_back(stack.back());
		stack.pop_back();
		stack.back() = n;
		return;
	}

	// declaration
	if (op == "let") {
		if (stack.size() - i != 2)
			throw std::vector<SyntaxError>{SyntaxError(n.token, "invalid declaration")};
		n.type = AST::NodeType::DECLARATION;
		n.members.emplace_back(stack.back());
		stack.pop_back();
		stack.back() = n;
		//
		return;
	}

	// generic infix operation
	if (stack.size() - i != 2 || stack.size() < 3)
		throw std::vector<SyntaxError>{SyntaxError(n.token, "invalid infix operator")};

	const AST r = stack.back();
	stack.pop_back(); // pop r
	stack.pop_back(); // pop n
	const AST l = stack.back();

	n.type = AST::NodeType::OPERATION;

	// if multiple members of same type, combine into bulk operation
	//  this makes dealing with associativity easier
	if (l.type == AST::NodeType::OPERATION && l.token.token == n.token.token)
		n.members = l.members;
	else
		n.members.emplace_back(l);

	if (r.type == AST::NodeType::OPERATION && r.token.token == n.token.token)
		n.members.insert(n.members.end(), r.members.begin(), r.members.end());
	else
		n.members.emplace_back(r);

	stack.back() = n;

	return;

}

static inline bool reduce_operators(std::vector<struct AST>& stack, const AST& n) {
	// dont reduce if given neg
	if (n.type == AST::NodeType::OPERATOR && n.token.token == "neg")
		return false;

	if (!isOperand(n)) {
		// if lookahead is an operator
		const int i = prev_operator(stack);
		if (i < 0)
			return false;
		const auto prec_p = op_prec.at(stack[i].token.token);
		const auto prec_n = op_prec.at(n.token.token);
		// reduce if it's lower precedence than prev operator
		if (prec_n <= prec_p) {
			reduce_operator(stack, i);
			return true;
		} else {
			return false;
		}
	}

	// assumed end of expr, reduce it before adding more
	if (n.type != AST::NodeType::LIST_OPEN && n.type != AST::PAREN_OPEN && isOperand(stack.back())) {
		reduce_operator(stack, prev_operator(stack));
		return true;
	}

	return false;
}

static inline bool reduce_containers(std::vector<AST>& stack) {
	if (stack.back().type == AST::NodeType::CONT_CLOSE) {
		// find nearest opener
		int i = stack.size() - 1;
		while (--i > 0)
			if (stack[i].type == AST::NodeType::LIST_OPEN ||
				stack[i].type == AST::NodeType::MACRO_OPEN ||
				stack[i].type == AST::NodeType::PAREN_OPEN)
				break;

		// handle lists
		if (stack[i].type == AST::NodeType::LIST_OPEN) {
			if (stack.back().token.token != "]")
				throw std::vector<SyntaxError>{
						SyntaxError(stack[i].token, "unclosed `[`"),
						SyntaxError(stack.back().token, "unexpected `)`"),
				};
			if (stack.size() - i > 3)
				throw std::vector<SyntaxError>{SyntaxError(stack[i].token, "invalid expression in list")};
			stack.pop_back();
			const AST n = stack.back();
			stack.pop_back();

			// list.members = comma separated values
			if ((n.type == AST::NodeType::OPERATION && n.token.token == ",") || n.type == AST::NodeType::COMMA_SERIES)
				n.type = AST::NodeType::LIST;
			else
				n = AST(AST::NodeType::LIST, stack.back().token, std::vector<AST>({n}));

			stack.back() = n;
			return true;
		}

		// handle parens
		if (stack[i].type == AST::NodeType::PAREN_OPEN) {
			if (stack.back().token.token != ")")
				throw std::vector<SyntaxError>{
						SyntaxError(stack[i].token, "unclosed `(`"),
						SyntaxError(stack.back().token, "unexpected `]`"),
				};
			if (stack.size() - i > 3)
				throw std::vector<SyntaxError>{SyntaxError(stack[i].token, "invalid expression in parens")};

			stack.pop_back();
			const AST e = stack.back();
			stack.pop_back();
			stack.back().type = AST::NodeType::PAREN_EXPR;
			stack.back().members.emplace_back(e);
			return true;
		}

		// handle macros
		if (stack[i].type == AST::NodeType::MACRO_OPEN) {
			if (stack.back().token.token != ")")
				throw std::vector<SyntaxError>{
					SyntaxError(stack[i].token, "unclosed `(:`"),
					SyntaxError(stack.back().token, "unexpected `]`"),
				};
			stack.pop_back();
			for (int m = i; m < stack.size(); m++)
				stack[i].members.emplace_back(stack[m])
			while (stack.size() > i + 1)
				stack.pop_back();
			stack.back().type == AST::NodeType::MACRO;

			return true;
		}
	}

	return false;
}

// macro calls			abc(123)
// bracket operator 	abc[123]
//
static inline bool reduce_invocations(std::vector<AST>& stack) {
	/* macro calls
	 * takes something that evaluates to a macro
	 * macro literal, paren expr,
	 */
}


static inline bool reduce(const std::vector<Token>& tokens, size_t& i, std::vector<AST>& stack, const AST& n) {
	if (stack.empty())
		return false;

	return
		reduce_operators(stack, n) ||
		reduce_containers(stack);

}

static inline bool can_shift(const AST& n) {
	if (n.type == AST::NodeType::INVALID)
		return false;

	return true;
}

AST parse(const std::vector<Token>& tokens) {
	std::vector<AST> stack;

	size_t i = 0;


	while (i < tokens.size()) {
		AST tok = next_node(tokens, i, stack);
		std::cout <<"Lookahead: " << debug_AST(tok) <<std::endl;
		do {
			std::cout <<"Stack: ";
			for (const AST& n : stack)
				std::cout <<debug_AST(n) << "   ";
			std::cout <<std::endl;
		} while (reduce(tokens, i, stack, tok));

		if (can_shift(tok))
			stack.emplace_back(tok);
	}

	std::cout <<"\nStack: ";
	for (const AST& n : stack)
		std::cout <<debug_AST(n) << "   ";
	std::cout <<std::endl;

	return stack.back();

	// if multi-char operator sequence try to reduce
	// always reduce containers
	//
}
