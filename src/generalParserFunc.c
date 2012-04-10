/*
 * @file generalParserFunc.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

//#include "include/uthash.h"
//#include "symboltable.h"
//#include <stdio.h>
//#include <stdarg.h>
//#include <stdlib.h>
#include <types.h>

expr expressionReturn(expr children) {
	children.lvalue = 0;
	return children;
}


