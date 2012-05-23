/**
 * @file main.h
 * @brief Contains the main functions and some resource handling
 */
/**
 * @mainpage DHBW compilerbau Praktikum
 *
 * This is the documentation of the compiler developed for the lecture compilerbau - Praktikum by Andi Hellmund in the 4th semester at DHBW Mannheim.
 *
 * @author Ulrich Ahrendt
 * @author Sebastian Nickel
 * @author Dirk Klostermann
 * @author Nicolai Ommer
 */
#ifndef _DHBWCC_MAIN_H
#define _DHBWCC_MAIN_H

#include "diag.h"
#include "resource_manager.h"

/** 
 * \struct cc_options
 * \brief Compiler options for DHBWCC.
 *
 * The global compiler options structure.
 */
typedef struct cc_options
{
  int print_ir;                 /**< Flag indicating if the IR should be printed */
  char *ir_file;                /**< The name of the IR file */

  char *input_file;             /**< The name of the input file */
  char *output_file;            /**< The name of the output file */

  int print_only_errors;
} cc_options_t;

extern cc_options_t cc_options; 

#endif
