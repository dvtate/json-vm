//
// Created by tate on 06-12-19.
//

#ifndef JSON_LISP_STATE_HPP
#define JSON_LISP_STATE_HPP

#include <unordered_map>
#include <vector>

#include "value.hpp"

class State_Exit {
	enum {
		// different break types
		B_LOOP,
		B_UP,
	} type;


	unsigned int depth;
};

class State {
public:
	typedef State_Exit Exit;

	//
	std::unordered_map<std::string, Value> vars;

	// stack based
	std::vector<Value> stack;


};

#endif //JSON_LISP_STATE_HPP