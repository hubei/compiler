/**
 * @file mips32gen.h
 * @brief Contains functions for generating assambler code
 *
 *  Created on: Mar 31, 2012
 *      Author: NicolaiO
 */

#ifndef MIPS32GEN_H_
#define MIPS32GEN_H_

#include "types.h"
#include <stdio.h>

int mips32gen(FILE*,irCode_t*,symbol_t*);


#endif /* MIPS32GEN_H_ */
