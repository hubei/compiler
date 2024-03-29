/**
 * @file diag.h
 * @brief Provides a dynamic error function
 */
#ifndef _DHBWCC_DIAG_H
#define _DHBWCC_DIAG_H
#include <stdarg.h>

typedef enum {
  OUT_OF_MEMORY = 0,    //!< OUT_OF_MEMORY
  FILE_ACCESS = 1,      //!< FILE_ACCESS
  PROCESS_RESOURCES = 2,//!< PROCESS_RESOURCES

  TYPE = 3,
  /* add new error codes above */
  NUM_OS_ERRORS         //!< NUM_OS_ERRORS
} os_error_t;

extern void fatal_os_error (os_error_t err, int err_no, const char *fn, const int line, const char *msg, ...);
void compilerError(int line, int exiting, const char *msg, ...);
#define FATAL_OS_ERROR(err,err_number,...) fatal_os_error(err, err_number, __FILE__, __LINE__, __Vh, A_ARGS__);

#endif
