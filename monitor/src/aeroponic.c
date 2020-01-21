#include "aeroponic.h"

int
set_processes(int processes[], char* str, int uptime)
{
	char* proc = NULL;

	proc = strtok(str, " ");
	for (int i = 0; i < MONITOR__PROCESS__MAX && proc != NULL; i++) {
	    debug("setting process: %s", proc);
	    if (processes[i] == atoi(proc)) /* Log failed process */
		log_info("Process %s failed after %d uptime", get_process_string(i), uptime);
	    processes[i] = atoi(proc); // regex FIXME
	    proc = strtok(NULL, " ");
	}

	return 0;
//error:
//	return -1;
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
	check(clear_gpio() == 0, "Failed to clear gpio");

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
		log_err("Unknown active processes");
	    }
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

        check(pipe(pipes) >= 0, "Pipe failed");

	check(fcntl(pipes[READ], F_SETFL, O_NONBLOCK) >= 0, "fcntl failed for pipe");

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
        int nread, proc_failures, initial_setup;
	int pipes[2], processes[AEROPONIC__PROCESS__MAX];
	int process_start_time;
	char process_buff[256];

	proc_failures = 0;
	initial_setup = true;
	process_start_time = time(NULL);
        check((processes[MONITOR] = start_monitor_child(pipes)) != -1, "Failed to start monitor child process");

int x = 0;
        for ( ; ; ) {
	    check(proc_failures < PROC_FAILURE_LIMIT, "Processes failing, need maintenance");
	    nread = read(pipes[READ], process_buff, sizeof(process_buff));
	    switch (nread) {
	    case -1:
		if (errno == EAGAIN) { /* pipe empty */
		    if (initial_setup == false && check_process_activity(processes) != 0) { /* Check if all processes are still active */
			log_err("Processes inactive, need aeroponic maintenance");
		    }
		    sleep(5);
		    continue;
		} else if (errno == EINVAL) {
		    log_err("Too much data being sent");
		} else {
		    log_err("read error");
		}
		break;
	    case 0:
		break;
	    default:
		debug("aeroponic receives: %s\n", process_buff);
		if (strncmp(process_buff, "ERROR:", strlen("ERROR:")) != 0) { /* solenoid or pump failed, updating processes array */
		    proc_failures++;
		    check(set_processes(processes, process_buff, (time(NULL) - process_start_time)) == 0, "Failed to set processes array");
		    initial_setup = false;
		    x++;
		} else { /* Monitor produced error Resetting processes */
		    debug("else");
		    proc_failures++;
		    check(kill_processes(processes) == 0, "Error killing child processes");
		    check((processes[MONITOR] = start_monitor_child(pipes)) != -1, "Failed to start monitor child process");
		    initial_setup = true;
		}
if (x > 5) {
kill_processes(processes);
exit(0);
}
	    }
        }

	kill_processes(processes);
        exit(0);
error:
	kill_processes(processes);
        exit(-1);
}

