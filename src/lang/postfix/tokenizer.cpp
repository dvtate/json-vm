
// Created by tate on 17-03-20.
//

#include <deque>
#include <string>
#include <vector>
#include <algorithm>
#include <istream>
#include <iostream>

#include "tokenizer.hpp"


static inline constexpr bool strcont(const char* s, const char c) {
	while (*s && *s != c)
		s++;
	return *s;
}
template <class T>
static inline bool quote_escaped(const T& buff, size_t offset) {
	unsigned char bs = 0;
	while (buff[offset--] == '\\' /* && offset */)
		bs++;
	return bs % 2;
}


template <class T, typename F>
static inline bool end_str(T& buff, size_t& i, const F read, const char start_c = '"') {

	do {
		while (++i < buff.size()) {
			if (buff[i] == start_c && !quote_escaped(buff, i)) {
				return true;
			}
		}
		// didnt find end of string... fetch next line...
		read();
	} while (i < buff.size());
	return false;
}


// ignore until newline
template <class T>
static inline bool end_line_comment(const T& s, size_t& i) {

	while (i < s.size() && s[i] != '\n')
		i++;
	// NOTE: assuming buffer end is also line end
	return true;
}


// ignore until end of multi-line comment
template <class T, class F> static inline bool
end_multi_comment(const T& buff,
		size_t& i,
		const F read)
{
	do {

		while (i < buff.size()) {
			i++;
			if (buff[i] == '/' && buff[i] - 1 == '*')
				return true;
		}
		// didn't find end of multi-comment

		// get next line
		read();

	// if not eof, scan again
	} while (i < buff.size());

	// !! syntax error unterminated multi-comment
	return false;
}

//
//static bool valid_number(std::string str)
//{
//	int i = 0, j = str.length() - 1;
//
//	// Handling whitespaces
//	while (i < str.length() && str[i] == ' ')
//		i++;
//	while (j >= 0 && str[j] == ' ')
//		j--;
//
//	if (i > j)
//		return 0;
//
//	// if string is of length 1 and the only
//	// character is not a digit
//	if (i == j && !(str[i] >= '0' && str[i] <= '9'))
//		return 0;
//
//	// If the 1st char is not '+', '-', '.' or digit
//	if (str[i] != '.' && str[i] != '+'
//		&& str[i] != '-' && !(str[i] >= '0' && str[i] <= '9'))
//		return 0;
//
//	// To check if a '.' or 'e' is found in given
//	// string. We use this flag to make sure that
//	// either of them appear only once.
//	bool flagDotOrE = false;
//
//	for (i; i <= j; i++) {
//		// If any of the char does not belong to
//		// {digit, +, -, ., e}
//		if (str[i] != 'e' && str[i] != '.'
//			&& str[i] != '+' && str[i] != '-'
//			&& !(str[i] >= '0' && str[i] <= '9'))
//			return 0;
//
//		if (str[i] == '.') {
//			// checks if the char 'e' has already
//			// occurred before '.' If yes, return 0.
//			if (flagDotOrE == true)
//				return 0;
//
//			// If '.' is the last character.
//			if (i + 1 > str.length())
//				return 0;
//
//			// if '.' is not followed by a digit.
//			if (!(str[i + 1] >= '0' && str[i + 1] <= '9'))
//				return 0;
//		}
//
//		else if (str[i] == 'e') {
//			// set flagDotOrE = 1 when e is encountered.
//			flagDotOrE = true;
//
//			// if there is no digit before 'e'.
//			if (!(str[i - 1] >= '0' && str[i - 1] <= '9'))
//				return 0;
//
//			// If 'e' is the last Character
//			if (i + 1 > str.length())
//				return 0;
//
//			// if e is not followed either by
//			// '+', '-' or a digit
//			if (str[i + 1] != '+' && str[i + 1] != '-'
//				&& (str[i + 1] >= '0' && str[i] <= '9'))
//				return 0;
//		}
//	}
//
//	/* If the string skips all above cases, then
//	it is numeric*/
//	return 1;
//}


template <class T> static inline bool
end_num(const T& buff, size_t& i) {
	size_t start = i;
	// starting in different base
	if (buff[i] == '0') {
		i++;
		if (i == buff.size())
			return true;
		// hex
		if (buff[i] == 'x' || buff[i] == 'X') {
			i++;
			while (i < buff.size() && strcont("012345789abcdef", buff[i]))
				i++;
			return true;
			// bin
		} else if (buff[i] == 'b' || buff[i] == 'B') {
			i++;
			while (i < buff.size() && (buff[i] == '1' || buff[i] == '0'))
				i++;
			return true;
			// oct
		} else if (strcont("01234567", buff[i])) {

			i++;
			while (i < buff.size() && strcont("01234567", buff[i]))
				i++;

			// dummy started dec number with a zero :thonk:
			if (buff[i] == '8' || buff[i] == '9') {
				i = start;
			} else
				return true;

		} // else it's dec but they stupid for starting with a zero
	}
	// dec

	// check integer part 123*.456e789
	while (i < buff.size() && strcont("0123456789", buff[i]))
		i++;

	// end of number
	if (buff[i] != 'e' && buff[i] != '.')
		return true;
	const bool only_int = buff[i] == 'e';
	i++;
	// next digit-sequence
	while (i < buff.size() && strcont("0123456789", buff[i]))
		i++;

	if (only_int || buff[i] != 'e')
		return true;

	// exponent number
	while (i < buff.size() && strcont("0123456789", buff[i]))
		i++;

	return true;
}
//static inline bool end_num(const std::string& s, size_t& i) {
//	size_t start = i;
//	if (s[i] == '0') {
//
//	}
//	while (i < s.length() && strcont("0123456789.exb-", s[i])) {
//		i++;
//	}
//	return true;
//}


//static inline void token_end(const std::string& s, size_t& i) {
//	static const char terminators[] = " .:(){}[]/*&|=^%$#@!~+-;?<>,\t\n\r\\";
//	while (i < s.length() && !strcont(terminators, s[i]))
//		i++;
//}

template <class T>
static inline void end_id(const T& buff, size_t& i) {
	static const char terminators[] = " .:(){}[]/*&|=^%$#@!~+-;?<>,\t\n\r\\";
	while (i < buff.size() && !strcont(terminators, buff[i]))
		i++;
}

template <class T>
static inline std::string stl_substr(const T& buff, size_t start, const size_t end) {
	std::string ret;
	ret.reserve(end-start);
	for (; start < end; start++)
		ret += buff[start];
	return ret;
}

template <class T, class F>
Token get_token(const T& buff, size_t& i, const F read) {
	// skip spaces
	while (i < buff.size() && isspace(buff[i]))
		i++;

	if (i == buff.size())
		return { Token::t::END, "" };

	const char c = buff[i];

	if (c == '/') {
		i++;
		// eof
		if (i == buff.size())
			return { Token::t::OPERATOR, "/" };

		// line-comment
		if (buff[i] == '/') {
			i++;
			end_line_comment(buff, i);
			return get_token(buff, i, read);
		}

		if (buff[i] == '*') {
			i++;
			end_multi_comment(buff, i, read);
			return get_token(buff, i, read);
		}

		// operator that starts with /
		return { Token::t::OPERATOR , std::string() + c };
	}

	// string literal
	if (c == '\'' || c == '"') {
		const size_t start = i;
		if (end_str(buff, i, read, c)) {
			i++;
			return {Token::t::STRING, stl_substr(buff, start, i) };
		} else
			return { Token::t::ERROR, "Unterminated string" };
	}

	// operator
	const char operators[] = "(){}[].,;:=|&+*-/%$#@!?^<>\\";
	if (strcont(operators, c)) {
		i++;
		return {Token::t::OPERATOR, std::string() + c};
	}

	// number
	if (strcont("0123456789.", c)) {
		size_t start = i;
		end_num(buff, i);
		return { Token::t::NUMBER, stl_substr(buff, start, i) };
	}

	// identifier
	size_t start = i;
	end_id(buff, i);
	return { Token::t::IDENTIFIER , stl_substr(buff, start, i) };
}

//Token tokenizer(const std::string& s, size_t& i) {
//	// skip spaces
//	while (i < s.length() && isspace(s[i])) i++;
//	if (i == s.length())
//		return { Token::t::END, "" };
//
//
//	const char c = s[i];
//	i++;
//	if (c == '/') {
//		// eof
//		if (i == s.length())
//			return { Token::t::OPERATOR, "/" };
//
//		// ignore line comment
//		if (s[i] == '/') {
//			i++;
//			end_line_comment(s, i);
//			return tokenizer(s, i);
//		}
//
//		// ignore multi-line comment
//		if (s[i] == '*') {
//			i++;
//			end_multi_comment(s, i);
//			return tokenizer(s, i);
//		}
//
//		// operator that starts with /
//		size_t start = i;
//		token_end(s, i);
//		return { Token::t::OPERATOR , s.substr(start, i - start ) };
//
//	}
//
//	if (c == '(' || c == ')')
//		return { Token::t::PAREN, std::string() + c };
//	if (c == '{' || c == '}')
//		return { Token::t::BRACE, std::string() + c };
//	if (c == '[' || c == ']')
//		return { Token::t::BRACKET, std::string() + c };
//	if (c == ',')
//		return { Token::t::COMMA, "," };
//	if (c == ':')
//		return { Token::t::COLON, ":" };
//	if (c == '.')
//		return { Token::t::DOT, "." };
//
//
//	if (c == '\'' || c == '"') {
//		const size_t start = i;
//		if (end_str(s, i, c)) {
//			i++;
//			return {Token::t::STRING, s.substr(start, i - start - 1)};
//		} else
//			return { Token::t::ERR, "Unterminated string" };
//	}
//
//	// valid start to a number
//	if(strcont("1234567890-.", c)) {
//		const size_t start = i - 1;
//		if (end_num(s, i))
//			return { Token::t::NUM, s.substr(start, i - start )};
//		else
//			i = start;
//	}
//
//	// generic operator
//	size_t start = i - 1;
//	token_end(s, i);
//	return { Token::t::OPERATOR , s.substr(start, i - start ) };
//
//}

// buffered read and tokenize on a line-by-line basis
std::vector<struct Token> tokenize_stream(std::istream& in) {
	std::deque<char> buff;

	// fetch next line from stream
	const auto read = [&] () {
		std::cout <<"read";
		std::string tmp;
		std::getline(in, tmp);
		for (auto c: tmp)
			buff.push_back(c);
	};

	std::vector<Token> ret;

	struct Token t;
	size_t i = 0;
	for (; ;) {
		t = get_token(buff, i, read);
		if (t.type == Token::t::END) {
			std::cout <<"new tok: " <<t.type <<':' <<t.token <<std::endl;

			// try to read next line
			buff.clear();
			i = 0;
			read();
			if (!buff.size())
				return ret;
		} else {
			std::cout <<"new tok: " <<t.type <<':' <<t.token <<std::endl;
			ret.emplace_back(t);
		}
	}
}


//std::vector<struct Token> tokenize_string(const std::string& in) {
//	// no more input to fetch
//	const auto read = [](){};
//
//}