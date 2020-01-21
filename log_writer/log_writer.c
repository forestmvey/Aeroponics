#include "log_writer.h"

void
die(struct Connection *conn,const char *message)
{

	if (errno) {
	    perror(message);
	} else {
	    printf("ERROR: %s\n", message);
	    if (conn != NULL)
		Database_close(conn);
	}
	exit(1);
}

void
Log_print(struct Log *lg)
{
	printf("Id: %d\nAeroponic uptime: %d\nMonitor uptime: %d\nProcess failures: %d\nErrors: %s\n", lg->id, lg->aeroponic_uptime, lg->monitor_uptime, lg->process_failures, lg->error);
}

void
Database_load(struct Connection *conn)
{
	int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
	if (rc != 1) {
	    die(conn, "Failed to load database.");
	}
}

struct Connection*
Database_open(const char *filename, char mode)
{
	struct Connection *conn = malloc(sizeof(struct Connection));
	if (!conn) {
	    die(conn, "Memory Error");
	}

	conn->db = malloc(sizeof(struct Database));
	if (!conn->db) {
	    die(conn, "Memory Error");
	}

	if (mode =='c') {
	    conn->file = fopen(filename, "w");
	} else {
	    conn->file = fopen(filename, "r+");
	    if (conn->file) {
		Database_load(conn);
	    }
	}

	if (!conn->file) {
	    die(conn, "Failed to open the file");
	}

	return conn;
}

void
Database_close(struct Connection *conn)
{
	if (conn) {
	    if (conn->file) {
		fclose(conn->file);
	    }
	    if (conn->db) {
		free(conn->db);
	    }
	    free(conn);
	}
}

void
Database_write(struct Connection *conn)
{
	rewind(conn->file);

	int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
	if (rc != 1) {
	    die(conn, "Failed to write to database.");
	}

	rc = fflush(conn->file);
	if (rc == -1) {
	    die(conn, "Cannot flush database.");
	}
}

void
Database_create(struct Connection *conn)
{
	int i = 0;

	for (i = 0; i < MAX_ROWS; i++) {
	    // make a prototype to initialize it
	    struct Log lg = { .id = i, .set = 0 };
	    //then just assign it
	    conn->db->rows[i] = lg;
	}
}

void
Database_set(struct Connection *conn, int id, int aeroponic_uptime,
 int monitor_uptime, int process_failures, const char *error)
{
	struct Log *lg = &conn->db->rows[id];
	if (lg->set) {
	    die(conn, "Already set, delete it first");
	}

	lg->set = 1;
	lg->aeroponic_uptime = aeroponic_uptime;
	lg->monitor_uptime = monitor_uptime;
	lg->process_failures = process_failures;

	// FIXME strncpy
//	char *res = strncpy(lg->error, error, MAX_DATA);
	if (snprintf(lg->error, MAX_DATA, "%s", error) >= MAX_DATA)
	    die(conn, "error copy failed!");
}

void
Database_get(struct Connection *conn, int id)
{
	struct Log *lg = &conn->db->rows[id];

	if (lg->set) {
	    Log_print(lg);
	} else {
	    die(conn, "ID is not set!");
	}
}

void
Database_delete(struct Connection *conn, int id)
{
	struct Log lg = { .id = id, .set = 0 };
	conn->db->rows[id] = lg;
}

void
Database_list(struct Connection *conn)
{
	int i = 0;
	struct Database *db = conn->db;

	for (i = 0; i < MAX_ROWS; i++) {
	    struct Log *cur = &db->rows[i];

	    if (cur->set) {
		Log_print(cur);
	    }
	    printf("----------------\n");
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 3) {
	    die(NULL, "USAGE: log_writer <dbfile> <action> [action params]");
	}

	char *filename = argv[1];
	char action = argv[2][0];
	struct Connection *conn = Database_open(filename, action);
	int id = 0;
	int aeroponic_uptime = 0;
	int monitor_uptime = 0;
	int process_failures = 0;

	if (argc > 3) id = atoi(argv[3]);
	if (argc > 4) {
	    aeroponic_uptime = atoi(argv[4]);
	    monitor_uptime = atoi(argv[5]);
	    process_failures = atoi(argv[6]);
	}
	if (id >= MAX_ROWS) die(conn, "There are not that many records.");

	switch(action) {
	case 'c':
	    Database_create(conn);
	    Database_write(conn);
	    break;
	case'g':
	    if (argc != 4) die(conn, "Need an id to get");
		Database_get(conn, id);
	    break;
	case 's':
	    if (argc != 9) {
			die(conn, "Need an id, error, and uptimes to set.");
	    }
	    Database_set(conn, id, aeroponic_uptime, monitor_uptime, process_failures, argv[7]);
	    Database_write(conn);
	    break;
	case 'd':
	    if (argc != 4) {
			die(conn, "Need an id to delete.");
	    }
	    Database_delete(conn, id);
	    Database_write(conn);
	    break;
	case 'l':
	    Database_list(conn);
	    break;
	default:
	    die(conn, "Invalid action: c=create, g=get, s=set, d=del, l=list");
	}

	Database_close(conn);
	return 0;
}
