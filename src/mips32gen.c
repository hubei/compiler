/*
 * mips32gen.c
 *
 *  Created on: Mar 31, 2012
 *      Author: NicolaiO
 */

#include "mips32gen.h"

int mips32gen(FILE *file,irCode *ircode) {
	if(file == NULL) {
		return 1;
	}
	if(ircode == NULL) {
		return 2;
	}


	return 42;
}

