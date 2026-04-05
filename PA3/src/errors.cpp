#include "errors.h"
#include <iostream>

void report_error(ErrorType type, int lineno) {
	std::cout << "Error at line " << lineno << ": ";

	switch (type) {
	case ERR_UNDECLARED_VAR:
		std::cout << "Variable used but not declared." << std::endl;
		break;
	case ERR_UNDECLARED_FUNC:
		std::cout << "Function called but not declared." << std::endl;
		break;
	case ERR_REDEF_VAR:
		std::cout << "Variable redeclared in the same scope." << std::endl;
		break;
	case ERR_REDEF_FUNC:
		std::cout << "Function redeclared." << std::endl;
		break;

	case ERR_TYPE_MISMATCH_ASSIGN:
		std::cout << "Type mismatch in assignment." << std::endl;
		break;
	case ERR_TYPE_MISMATCH_OP:
		std::cout << "Type mismatch in binary operation." << std::endl;
		break;
	case ERR_TYPE_MISMATCH_RETURN:
		std::cout << "Return value does not match function return type." << std::endl;
		break;

	case ERR_FUNC_ARG_COUNT:
		std::cout << "Function argument count mismatch." << std::endl;
		break;
	case ERR_FUNC_ARG_TYPE:
		std::cout << "Function argument type mismatch." << std::endl;
		break;
	case ERR_NOT_A_FUNCTION:
		std::cout << "Attempted to call a non-function identifier." << std::endl;
		break;

	case ERR_NOT_AN_ARRAY:
		std::cout << "Attempted to index a non-array identifier." << std::endl;
		break;
	case ERR_ARRAY_INDEX_TYPE:
		std::cout << "Array index must be an integer." << std::endl;
		break;

	default:
		std::cout << "Unknown semantic error." << std::endl;
	}
}
