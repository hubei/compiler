/*
 * @file generalParserFunc.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

#include "types.h"

expr_t expressionReturn(expr_t children) {
	children.lvalue = 0;
	return children;
}


