//
// Created by tate on 06-12-19.
//

#ifndef JSON_LISP_OPERATOR_HPP
#define JSON_LISP_OPERATOR_HPP

struct Operator {

	// used in bytecode text representation
	// ie - "add"
	const char* name;

	// used in normal language, common text symbol
	// ie - "+"
	const char* symbol;

	


};

#endif //JSON_LISP_OPERATOR_HPP
