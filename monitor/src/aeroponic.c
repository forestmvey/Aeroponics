#include "aeroponic.h"

/*
* Kill all child processes
*/
int
kill_processes(int processes[])
{

}

pid_t
start_monitor_child(int pipes[])
{
        char p_write[10];
        pid_t pid;

        check(pipe(pipes) != -1, "Pipe failed");

        pid = fork();
        if (pid < 0) {
            log_err("Fork failed");
        } else if (pid == 0) { /* Child */
	    check(snprintf(p_write, sizeof(p_write), "%d", pipes[WRITE]) < (int)sizeof(p_write), "snprintf truncated");
            close(pipes[READ]);
            execl("./bin/monitor", "./monitor", p_write, (char *) NULL);
            exit(0);
        } else if (pid > 0) { /* Parent */
            close(pipes[WRITE]);
            return pid;
        }

error:
        return (pid_t)-1;

}

int
main()
{
        int nread;
	int pipes[2];
	char process_buff[256];
	pid_t childpid;

        childpid = start_monitor_child(pipes);

        for ( ; ; ) {

	    nread = read(pipes[READ], process_buff, sizeof(process_buff));
	    switch (nread) {
	    case -1:
		if (errno == EAGAIN) { /* pipe empty */
		    printf("pipe empty\n");
		    continue;
		} else if (errno == EINVAL) {
		    log_err("Too much data being sent");
		} else {
		    log_err("read error");
		}
	    case 0:
//		close(pipes[READ]);
	    default:
		printf("aeroponic receive: %s\n", process_buff);
		exit(0);
//	        close(pipes[READ]);
	    }

	    if (strncmp(process_buff, "", 1) == 0) /* No new items to insert into database */
		continue;

	    sleep(10);
        }

        exit(0);
error:
        exit(-1);
}

