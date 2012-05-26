/**
 * @file diag.h
 * @brief Provides a dynamic error function
 */
#ifndef _DHBWCC_DIAG_H
#define _DHBWCC_DIAG_H

typedef enum {
  OUT_OF_MEMORY = 0,    //!< OUT_OF_MEMORY
  FILE_ACCESS = 1,      //!< FILE_ACCESS
  PROCESS_RESOURCES = 2,//!< PROCESS_RESOURCES
  /* add new error codes above */
  NUM_OS_ERRORS         //!< NUM_OS_ERRORS
} os_error_t;

extern void fatal_os_error (os_error_t err, int errno, const char *fn, const int line, const char *msg, ...);
void compilerError(int line, int exiting, const char *msg, ...);
#define FATAL_OS_ERROR(err,errno,...) fatal_os_error(err, errno, __FILE__, __LINE__, __VA_ARGS__);

#endif
