//
// Created by tate on 06-12-19.
//

#ifndef JSON_LISP_VALUE_HPP
#define JSON_LISP_VALUE_HPP


#include <memory>			// shared_ptr
#include <iostream>			// debugging

#include "../libs/json.hpp"
#include "block.hpp"

// might switch one day
using json_t = nlohmann::json;


class Value {
public:

	enum {
		EMPTY,
		JSON,
		REF,
		MACRO,
	} type;

	union {
		json_t* j;
		std::shared_ptr<Value> ref;
		Block* b;
	} v;

	static std::shared_ptr<Value> null;


};


#endif //JSON_LISP_VALUE_HPP
