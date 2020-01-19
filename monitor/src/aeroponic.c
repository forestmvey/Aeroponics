#include "aeroponic.h"

/*
 * Kill all child processes
 */
int
kill_processes(int process[])
{
	struct stat sts;
	char proc_string[12];

	for (int i = 0; i < AEROPONIC__PROCESS__MAX; i++) {
	    check(snprintf(proc_string, sizeof(proc_string), "/proc/%d", process[i]) < (int)sizeof(proc_string), "snprintf truncated");

	    if (stat(proc_string, &sts) == -1 && errno == ENOENT) { /* Process doesn't exist */
		continue;
	    } else {
		kill(process[i], SIGKILL);
	    }
	}

	return 0;
error:
	return -1;
}

/*
 * Check if any processes are showing errors
 */
int
check_process_activity(int process[])
{
	struct stat sts;
	char proc_string[12];

	for (int i = 0; i < AEROPONIC__PROCESS__MAX; i++) {
	    check(snprintf(proc_string, sizeof(proc_string), "/proc/%d", process[i]) < (int)sizeof(proc_string), "snprintf truncated");

	    if (stat(proc_string, &sts) == -1 && errno == ENOENT) { /* Process doesn't exist */
		log_err("error logging ");
	    }
//	    if // new error logs restart
	}

	return 0;
error:
	return -1;
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
	int pipes[2], processes[AEROPONIC__PROCESS__MAX];
//	size_t process_start_time;
	char process_buff[256];
	pid_t childpid;

//	process_start_time = (size_t)time(NULL);
        childpid = start_monitor_child(pipes);
	check(childpid != -1, "Failed to start monitor child process");
	processes[MONITOR] = childpid;

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

	    if (check_process_activity(processes) != 0) {
		kill_processes(processes);
		start_monitor_child(pipes);
	    }

	    sleep(30);
        }

	kill_processes(processes);
        exit(0);
error:
	kill_processes(processes);
        exit(-1);
}

