#ifndef __logging_h__
#define __logging_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "common.h"

#ifndef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", \
	__FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(F, M, ...) fprintf(F, \
	"[ERROR] (%s:%d: errno: %s)" M "\n", __FILE__, __LINE__,\
	clean_errno(), ##__VA_ARGS__); goto error;\
	fclose(F);

#define log_process(F, M, ...) fprintf(F, \
	"[PROCESS] (%s:%d: errno: %s)" M "\n",\
	##__VA_ARGS__);\
	fclose(F);

#define log_info(F, M, ...) fprintf(F, "[INFO] (%s:%d) " M "\n",\
	__FILE__, __LINE__, ##__VA_ARGS__);\
	fclose(F);

#define check(A, F, M, ...) if(!(A)) {\
	log_err(F, M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(F, M, ...) { log_err(F, M, ##__VA_ARGS__);\
	errno=0; goto error;}

#define check_mem(A, F) check((A, F), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__);\
	errno=0; goto error; }

#endif
