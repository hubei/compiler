#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
//#include "symboltable.h"
#include "parser.h"
#include "address_code.h"
#include "mips32gen.h"
#include "symboltable.h"
#include "diag.h"
#include <stdarg.h>

#include "generalParserFunc.h"

#include "main.h"

/** define yyparse function, which is defined by bison, but not included and thus causes a warning */
void yyparse();

/** get a reference to the yyin und yyout variables for flex */
extern FILE *yyin;
extern FILE *yyout;

/** default return code is 0 (success) */
int errorCode = 0;

/* Constants */
static const char *C_EXT = ".c";
static const char *IR_EXT = ".ir";
static const char *OUTPUT_EXT = ".s";

cc_options_t cc_options = { .print_ir = 0, .ir_file = NULL, .input_file = NULL,
		.output_file = NULL };

/**
 * @brief Duplicate the given string
 * This function is standard in some compilers, but not all...
 * This is why it produces warnings...
 * As it it is a very simple function, it is just defined here
 * to avoid the warnings
 *
 * @param str string to duplicate
 * @return reference to the duplicated string
 */
char *strdup(const char *str) {
	int n = strlen(str) + 1;
	char *dup = malloc(n);
	if (dup == NULL) {
		fatal_os_error(OUT_OF_MEMORY, 1, __FILE__, __LINE__, "");
	}
	if (dup) {
		strcpy(dup, str);
	}
	return dup;
}

/** 
 * \brief Print the help.
 * \param prg_name The file name of the executable.
 */
void print_usage(const char *prg_name) {
	fprintf(stderr, "Usage: %s [-p] [-o output] [-h] file\n", prg_name);
}

/**
 * \brief Determines if the given filename has the file extension 'ext'.
 * \param file The filename to be checked.
 * \param ext The extension to check for.
 * \return A flag indicating if the filename has the specified file
 *         extension.
 *         0 = file has not the specified file extension
 *         1 = file has the specified file extension
 *
 *         It returns 0 if any of the input parameters is NULL.
 */
int has_file_extension(const char *file, const char *ext) {

	if (file == NULL || ext == NULL)
		return 0;

	size_t file_len = strlen(file);
	size_t ext_len = strlen(ext);

	if (ext_len > file_len) {
		return 0;
	} else {
		return !strncmp(file + file_len - ext_len, ext, ext_len);
	}
}

/**
 * \brief Determines the basename of the file, though without
 *        file extension.
 * \param file The file name.
 * \return The basename of the specified file. If the file name
 *         does not contain any extension, the input filename is
 *         returned. However, the string returned by this function
 *         is always a newly allocated memory block (malloc)  which 
 *         needs to be freed by the caller. 
 *
 *         It returns NULL if the input parameter is NULL or if 
 *         memory could not be allocated.
 */
char *get_file_basename(const char *file) {
	size_t baselen;
	char *filebase, *pos_ext;

	if (file == NULL)
		return NULL;

	pos_ext = strrchr(file, '.');

	if (pos_ext != NULL) {
		/* The file name has a .xx extension */
		baselen = pos_ext - cc_options.input_file;

		filebase = malloc((baselen + 1) * sizeof(*filebase));
		if (filebase == NULL) {
			fatal_os_error(OUT_OF_MEMORY, 1, __FILE__, __LINE__, "");
			return NULL;
		}
		strncpy(filebase, file, baselen);
		filebase[baselen] = '\0';
	} else {
		/* No file extensions found */
		char *tmp = strdup(file);
		if (tmp == NULL) {
			fatal_os_error(OUT_OF_MEMORY, 1, __FILE__, __LINE__, "");
			return NULL;
		}
		return tmp;
	}

	return filebase;
}

/**
 * \brief Appends the 'ext' to the file basename 'filebase'. 
 * \param filebase The file basename.
 * \param ext The file extension.
 * \return A filename with extension 'ext'. The returned string
 *         is always a newly allocated memory block (malloc) 
 *         that needs to be freed by the caller.
 * 
 *         It returns NULL if 'filebase' is NULL or memory could not
 *         be allocated. It returns filebase if 'ext' is NULL.
 */
char *get_filename_with_ext(const char *filebase, const char *ext) {

	if (filebase == NULL)
		return NULL;
	if (ext == NULL) {
		char *tmp = strdup(filebase);
		if (tmp == NULL) {
			fatal_os_error(OUT_OF_MEMORY, 1, __FILE__, __LINE__, "");
			return NULL;
		}
		return tmp;
	}

	size_t extlen = strlen(ext);
	size_t baselen = strlen(filebase);
	size_t strlen = baselen + extlen + 1;

	char *file = malloc(strlen * sizeof(*file));
	if (file == NULL) {
		fatal_os_error(OUT_OF_MEMORY, 1, __FILE__, __LINE__, "");
		return NULL;
	}
	sprintf(file, "%s%s", filebase, ext);

	return file;
}

/**
 * \brief Free memory allocated in options structure.
 */
void free_options(void *data) {
	if (cc_options.ir_file != NULL)
		free(cc_options.ir_file);
	if (cc_options.input_file != NULL)
		free(cc_options.input_file);
	if (cc_options.output_file != NULL)
		free(cc_options.output_file);
}

/** 
 * \brief Process compiler options.
 * \param argc The number of input parameters.
 * \param argv The input parameter strings.
 * \return Indicates if processing was successful. 
 *         0 = processing successful
 *         1 = processing not successful
 *
 * Processes the input options passed to the compiler
 * and fill the compiler options structure as appropriate.
 * 
 * The following options are supported:
 *  -h: print help
 *  -p: print the IR to a file
 *  -t: test modus, only success/failure log
 *  -o: the output file name (different from 'input'.o)
 */
int process_options(int argc, char *argv[]) {
	int opt;
	int ret = 0;

	/* add a handler to resource manager to free resources
	 * in the case of an error during option processing */
	rm_register_handler(&resource_mgr, free_options, NULL);

	while ((opt = getopt(argc, argv, "hpto:")) != -1) {
		switch (opt) {
		case 'p':
			cc_options.print_ir = 1;
			break;
		case 't':
			/* fewer logs, for automated testing */
			cc_options.print_only_errors = 1;
			break;
		case 'o':
			/* output file */
			cc_options.output_file = strdup(optarg);
			if (cc_options.output_file == NULL) {
				fatal_os_error(OUT_OF_MEMORY, 1, __FILE__, __LINE__, "");
				return 1;
			}
			break;
		case 'h':
			/* print help */
			print_usage(argv[0]);
			rm_cleanup_resources(&resource_mgr);
			exit(EXIT_SUCCESS);

		default: /* '?' */
			/* print usage */
			fprintf(stderr, "ERROR: unknown parameter: %s\n", argv[optind]);
			print_usage(argv[0]);
			return 1;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "ERROR: missing input file\n");
		print_usage(argv[0]);
		ret = 1;
	} else if (optind < argc - 1) {
		fprintf(stderr, "ERROR: too many input files\n");
		print_usage(argv[0]);
		ret = 1;
	} else {
		cc_options.input_file = strdup(argv[optind]);
		if (cc_options.input_file == NULL) {
			fatal_os_error(OUT_OF_MEMORY, 1, __FILE__, __LINE__, "");
			return 1;
		}

		char *filebase = get_file_basename(cc_options.input_file);
		;
		if (filebase == NULL) {
			return 1;
		}

		if (!has_file_extension(cc_options.input_file, C_EXT)) {
			fprintf(stderr, "ERROR: no C file (.c) as input\n");
			ret = 1;
		} else {
			/* The file name has a valid .c extension */
			if (cc_options.output_file == NULL) {
				/* create output file name <input>.o */
				cc_options.output_file = get_filename_with_ext(filebase,
						OUTPUT_EXT);
				if (cc_options.output_file == NULL) {
					ret = 1;
				}
			}
			if (cc_options.print_ir == 1) {
				/* create IR file name <input>.ir */
				cc_options.ir_file = get_filename_with_ext(filebase, IR_EXT);
				if (cc_options.ir_file == NULL) {
					ret = 1;
				}
			}
		}

		free(filebase);
	}

	return ret;
}

/**
 * \brief Entry point.
 * \param argc The number of input parameters.
 * \param argv The input parameters.
 */
int main(int argc, char *argv[]) {

	/* the resource manager must be initialized before any
	 * further actions are implemented */
	rm_init(&resource_mgr);

	if (process_options(argc, argv) == 1) {
		rm_cleanup_resources(&resource_mgr);
		exit(EXIT_FAILURE);
	}

	if (cc_options.print_only_errors != 1) {
		printf("Input: %s\n", cc_options.input_file);
		printf("Output: %s\n", cc_options.output_file);
		printf("IR: %s\n", cc_options.ir_file);
	}

	yyin = fopen(cc_options.input_file, "r");
	if (!yyin) {
		fprintf(stderr,
				"Input file could not be opened for reading. Maybe, file does not exist?");
	} else {
		setSymbolTable(createSymbol());
		yyparse();
		fclose(yyin);
		irCode_t* ircode = NULL;

		if (cc_options.ir_file != NULL) {
			FILE *irFile;
			irFile = fopen(cc_options.ir_file, "w+");

			// Test symbolTable
			print_symTab(irFile);

			// get ir code and print it into irFile
			struct func_t *func, *tmp;
			HASH_ITER(hh, getSymbolTable()->symFunc, func, tmp) {
				if (func->symbol != NULL) {
					fprintf(irFile, "Function %s:\n", func->id);
					printIRCode(irFile, func->symbol->ircode);
				}
			}

			fclose(irFile);
		}

		yyout = fopen(cc_options.output_file, "w+");
		if (!yyout) {
			fprintf(stderr,
					"Output file could not be opened for writing. Maybe, file does not exist?");
		} else {
			int ret = mips32gen(yyout, ircode, getSymbolTable());
			if (ret != 0) {
				fprintf(stderr, "Error generating mips32 code with code: %d\n",
						ret);
			}
			fclose(yyout);
		}
	}

	clean_symbol(getSymbolTable());
//	clean_all_expr();
	if (cc_options.print_only_errors != 1) {
		fprintf(stdout, "\nFinished.");
	} else {
		printf("Finished: %s\n", cc_options.input_file);
	}

	rm_cleanup_resources(&resource_mgr);
	return errorCode;
}

