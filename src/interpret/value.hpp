//
// Created by tate on 06-12-19.
//

#ifndef JSON_LISP_VALUE_HPP
#define JSON_LISP_VALUE_HPP


#include <memory>			// shared_ptr
#include <iostream>			// debugging

#include "../libs/json.hpp"


// might switch one day
using json_t = nlohmann::json;


class Value {
public:

	enum {
		EMPTY,
		JSON,
		REF,
	} type;

	union {
		json_t* j;
		std::shared_ptr<Value> ref;
	} v;

	static std::shared_ptr<Value> null;


};


#endif //JSON_LISP_VALUE_HPP
