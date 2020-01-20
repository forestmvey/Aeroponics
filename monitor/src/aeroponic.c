#include "aeroponic.h"

int
set_processes(int processes[], char* str)
{
	char* proc = NULL;

	proc = strtok(str, " ");
	for (int i = 0; i < MONITOR__PROCESS__MAX && proc != NULL; i++) {
	    processes[i] = atoi(proc); // regex FIXME
	    proc = strtok(NULL, " ");
	}

	return 0;
error:
	return -1;
}

/*
 * Kill all child processes
 */
int
kill_processes(int process[])
{
	struct stat sts;
	char proc_string[24];

	for (int i = 0; i < AEROPONIC__PROCESS__MAX; i++) {
	    check(snprintf(proc_string, sizeof(proc_string), "/proc/%d", process[i]) < (int)sizeof(proc_string), "snprintf truncated");

	    if (stat(proc_string, &sts) == -1 && errno == ENOENT) { /* Process doesn't exist */
		printf("process %d doesn't exist\n", process[i]);
		continue;
	    } else {
		printf("killing process: %d\n", process[i]);
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
printf("checking process: %d\n", process[i]);
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


//	process_start_time = (size_t)time(NULL);
        check((processes[MONITOR] = start_monitor_child(pipes)) != -1, "Failed to start monitor child process");
printf("process monitor: %d\n", processes[MONITOR]);
int x = 0;
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
		break;
	    case 0:
//		close(pipes[READ]);
		break;
	    default:
		log_info("aeroponic receive: %s\n", process_buff);
		if (strncmp(process_buff, "ERROR:", strlen("ERROR:")) != 0) {
		    check(set_processes(processes, process_buff) == 0, "Failed to set processes array");
		    x++;
		} else {
		    log_info("%s", process_buff);
		    check(kill_processes(processes) == 0, "Error killing child processes");
		    check((processes[MONITOR] = start_monitor_child(pipes)) != -1, "Failed to start monitor child process");
printf("process monitor: %d\n", processes[MONITOR]);
		}
if (x > 2) {
kill_processes(processes);
exit(0);
}
	    }

	    if (check_process_activity(processes) != 0) {
		log_err("Processes inactive, need aeroponic maintenance");
//		kill_processes(processes);
//		start_monitor_child(pipes);
	    }

	    sleep(1);//
        }

	kill_processes(processes);
        exit(0);
error:
	kill_processes(processes);
        exit(-1);
}

