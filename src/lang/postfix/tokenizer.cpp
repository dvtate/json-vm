//
// Created by tate on 17-03-20.
//

#include "tokenizer.hpp"


static inline constexpr bool strcont(char* s, const char c) {
	while (*s && *s != c)
		s++;
	return *s;
}


static const char parens[] = "()";
static const char parens[] = "()";

static inline bool quote_escaped(const std::string& buff, size_t offset) {
	unsigned char bs = 0;
	while (buff[offset--] == '\\' /* && offset */)
		bs++;
	return bs % 2;
}

// return: true if ended, false on eof
static inline bool end_str(const std::string& s, size_t& i, const char start_c = '"') {
	while (++i < s.length()) {
		if (s[i] == start_c && !quote_escaped(s, i)) {
			return true;
		}
	}
	return false;
}


// ignore until newline
static inline void end_line_comment(const std::string& s, size_t& i) {
	while (s[i] != '\n' && i < s.length())
		i++;
	return;
}


// ignore until end of comment
static inline void end_multi_comment(const std::string& s, size_t& i) {
	while (i < s.length()) {
		i++;
		if (s[i] == '/' && s[i] - 1 == '*')
			break;
	}
}

const char* builtin_ops[] = {

};


static inline void next_space(const std::string& s, size_t& i) {
	while (i < s.length() && !isspace(s[i]))
		i++;
}

Token tokenizer(const std::string& s, size_t& i) {
	// skip spaces
	while (i < s.length() && isspace(s[i])) i++;
	if (i == s.length())
		return { Token::t::NONE, "" };


	const char c = s[i];
	if (c == '/') {
		i++;
		// eof
		if (i == s.length())
			return { Token::t::OPERATOR, "/" };

		// ignore line comment
		if (s[i] == '/') {
			i++;
			end_line_comment(s, i);
			return tokenizer(s, i);
		}

		// ignore multi-line comment
		if (s[i] == '*') {
			i++;
			end_multi_comment(s, i);
			return tokenizer(s, i);
		}

		size_t start = i;
		next_space(s, i);
		return { Token::t::OPERATOR }

	}

	if (c == '(' || c == ')')
		return { Token::t::PAREN, std::to_string(c) };
	if (c == )
}
