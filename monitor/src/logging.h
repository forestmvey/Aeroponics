#ifndef __logging_h__
#define __logging_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "common.h"

#ifndef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) syslog(LOG_INFO, "DEBUG %s:%d: " M "\n", \
	__FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) log_write(\
	"[ERROR] (%s:%s) (%s:%d: errno: %s)" M "\n", __DATE__, __TIME__, __FILE__, __LINE__, \
	clean_errno(), ##__VA_ARGS__); goto error; \

#define log_sys(M, ...) syslog(LOG_ERR, \
	"[ERROR] (%s:%s) (%s:%d: errno: %s)" M "\n", __DATE__, __TIME__, __FILE__, __LINE__, \
	clean_errno(), ##__VA_ARGS__); goto error; \

#define log_info(M, ...) log_write("[INFO] (%s:%s) (%s:%d) " M "\n", \
	__DATE__, __TIME__, __FILE__, __LINE__, ##__VA_ARGS__); \

#define log_process(...) log_write("[PROCESS] (%s:%s) (UPTIME: aeroponic:%zu monitor:%zu pump:%zu solenoid:%zu) (FAILURES:%d)\n", \
	__DATE__, __TIME__, ##__VA_ARGS__); \

#define check(A, M, ...) if(!(A)) { \
	log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_sys(A, M, ...) if(!(A)) { \
	log_sys(M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__); \
	errno=0; goto error;}

#define check_mem(A) check((A), "[ERROR] Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); \
	errno=0; goto error; }

#endif
