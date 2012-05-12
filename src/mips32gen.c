/*
 * mips32gen.c
 *
 *  Created on: Mar 31, 2012
 *      Author: NicolaiO
 */

#include "mips32gen.h"

int mips32gen(FILE *file,irCode_t *ircode, symbol_t *symbolTable) {
	if(file == NULL) {
		return 1;
	}
//	if(ircode == NULL) {
//		return 2;
//	}
	if(symbolTable == NULL) {
		return 3;
	}

	// global data section
	fprintf(file,".data\n");

	struct var_t *k, *tmp;
	HASH_ITER(hh, symbolTable->symVar, k, tmp) {
		fprintf(file,"%s:\n",k->id);
		fprintf(file,"\t.word 0");
		if(k->size>0)
			fprintf(file," : %d",k->size);
		int align = k->width;
		if(k->size!=0) align /= k->size;
		fprintf(file,"\n\t.align %d\n", align);
	}
	fprintf(file, "\n");

	// global text section
	fprintf(file, ".text\n\n");
	// TODO do not know what to do here yet...

	// define start point
	fprintf(file, "_start:\n\tJAL main\n\n");

	// code
	struct func_t *f, *tmp2;
	HASH_ITER(hh, symbolTable->symFunc, f, tmp2) {
		fprintf(file, "%s:\n",f->id);

		// TODO Basti here is the main work
		fprintf(file, "#-> function prolog <-#\n");

		fprintf(file, "#-> local variables <-#\n");

		fprintf(file, "#-> function epilog <-#\n");

		fprintf(file, "\n");
	}


	return 42;
}

char* generate(irCode_t* ircode) {
	return NULL;
}

char* getComment() {
	return NULL;
}

