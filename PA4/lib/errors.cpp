#include "errors.h"
#include <cstdlib>
#include <iostream>

using namespace std;

void report_error(ErrorType type, int lineno) {
	cout << "Error at line " << lineno << ": ";

	switch (type) {
	case ERR_UNDECLARED_VAR:
		cout << "Variable used but not declared." << endl;
		break;
	case ERR_UNDECLARED_FUNC:
		cout << "Function called but not declared." << endl;
		break;
	case ERR_REDEF_VAR:
		cout << "Variable redeclared in the same scope." << endl;
		break;
	case ERR_REDEF_FUNC:
		cout << "Function redeclared." << endl;
		break;

	case ERR_TYPE_MISMATCH_ASSIGN:
		cout << "Type mismatch in assignment." << endl;
		break;
	case ERR_TYPE_MISMATCH_OP:
		cout << "Type mismatch in binary operation." << endl;
		break;
	case ERR_TYPE_MISMATCH_RETURN:
		cout << "Return value does not match function return type." << endl;
		break;

	case ERR_FUNC_ARG_COUNT:
		cout << "Function argument count mismatch." << endl;
		break;
	case ERR_FUNC_ARG_TYPE:
		cout << "Function argument type mismatch." << endl;
		break;
	case ERR_NOT_A_FUNCTION:
		cout << "Attempted to call a non-function identifier." << endl;
		break;

	case ERR_NOT_AN_ARRAY:
		cout << "Attempted to index a non-array identifier." << endl;
		break;
	case ERR_ARRAY_INDEX_TYPE:
		cout << "Array index must be an integer." << endl;
		break;

	default:
		cout << "Unknown semantic error." << endl;
	}
}
