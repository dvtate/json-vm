//
// Created by tate on 17-03-20.
//

#include "tokenizer.hpp"


static inline constexpr bool strcont(const char* s, const char c) {
	while (*s && *s != c)
		s++;
	return *s;
}

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
static inline bool end_line_comment(const std::string& s, size_t& i) {
	while (i < s.length() && s[i] != '\n')
		i++;
	return true;
}


// ignore until end of comment
static inline bool end_multi_comment(const std::string& s, size_t& i) {
	while (i < s.length()) {
		i++;
		if (s[i] == '/' && s[i] - 1 == '*')
			return true;
	}
	return false;
}

static inline bool end_num(const std::string& s, size_t& i) {
	size_t start = i;
	while (strcont("0123456789.exb-", s[i])) {
		i++;
	}
	return true;
}


static inline void token_end(const std::string& s, size_t& i) {
	static const char terminators[] = " .:(){}[]/*&|=^%$#@!~+-;?<>,\t\n\r\\";
	while (i < s.length() && !strcont(terminators, s[i]))
		i++;
}

Token tokenizer(const std::string& s, size_t& i) {
	// skip spaces
	while (i < s.length() && isspace(s[i])) i++;
	if (i == s.length())
		return { Token::t::END, "" };


	const char c = s[i];
	i++;
	if (c == '/') {
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

		// operator that starts with /
		size_t start = i;
		token_end(s, i);
		return { Token::t::OPERATOR , s.substr(start, i - start ) };

	}

	if (c == '(' || c == ')')
		return { Token::t::PAREN, std::string() + c };
	if (c == '{' || c == '}')
		return { Token::t::BRACE, std::string() + c };
	if (c == '[' || c == ']')
		return { Token::t::BRACKET, std::string() + c };
	if (c == ',')
		return { Token::t::COMMA, "," };
	if (c == ':')
		return { Token::t::COLON, ":" };
	if (c == '.')
		return { Token::t::DOT, "." };


	if (c == '\'' || c == '"') {
		const size_t start = i;
		if (end_str(s, i, c)) {
			i++;
			return {Token::t::STRING, s.substr(start, i - start - 1)};
		} else
			return { Token::t::ERR, "Unterminated string" };
	}

	// valid start to a number
	if(strcont("1234567890-.", c)) {
		const size_t start = i - 1;
		if (end_num(s, i))
			return { Token::t::NUM, s.substr(start, i - start )};
		else
			i = start;
	}

	// generic operator
	size_t start = i - 1;
	token_end(s, i);
	return { Token::t::OPERATOR , s.substr(start, i - start ) };

}
