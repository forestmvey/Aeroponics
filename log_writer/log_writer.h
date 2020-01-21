#ifndef _log_writer_h
#define _log_writer_h

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 1024
#define MAX_ROWS 31


struct Log {
	int id;
	int set;
	int aeroponic_uptime;
	int monitor_uptime;
	int process_failures;
	char error[MAX_DATA];
};

struct Database {
	struct Log rows[MAX_ROWS];
};

struct Connection {
	FILE *file;
	struct Database *db;
};


__BEGIN_DECLS

void
Database_close(struct Connection*);

void
die(struct Connection*,const char*);

void
Log_print(struct Log*);

void
Database_load(struct Connection*);

struct Connection*
Database_open(const char*, char);

void
Database_close(struct Connection*);

void
Database_write(struct Connection*);

void
Database_create(struct Connection*);

void
Database_set(struct Connection*, int, int,
 int, int, const char*);

void
Database_get(struct Connection*, int);

void
Database_delete(struct Connection*, int);

void
Database_list(struct Connection*);

__END_DECLS

#endif
