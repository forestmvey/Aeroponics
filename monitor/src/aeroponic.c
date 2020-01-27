#include "aeroponic.h"

int
set_processes(int* process, char* str, size_t* process_start_time, int* proc_failures)
{
	char* proc = NULL;

	proc = strtok(str, " ");
	for (int i = 0; i < MONITOR__PROCESS__MAX && proc != NULL; i++) {
	    debug("setting process: %s", proc);
	    if (process[i] != atoi(proc) && process[i] != 0) {
		log_info("Process %s failed after %zu uptime", get_process_string(i), ((size_t)time(NULL) - process_start_time[i]));
		process_start_time[i] = (size_t)time(NULL);
		proc_failures++;
	    }
	    process[i] = atoi(proc); // regex FIXME
	    proc = strtok(NULL, " ");
	}
	check(clear_gpio() == 0, "Failed to clear gpio");

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
	    check(snprintf(proc_string, sizeof(proc_string), "/proc/%d", process[i]) < (int)sizeof(proc_string),
		"snprintf truncated");

	    if (stat(proc_string, &sts) == -1 && errno == ENOENT) { /* Process doesn't exist */
		log_info("process %d doesn't exist\n", process[i]);
		continue;
	    } else {
		log_info("killing process: %d\n", process[i]);
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
	    check(snprintf(proc_string, sizeof(proc_string), "/proc/%d", process[i]) < (int)sizeof(proc_string),
		"snprintf truncated");

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
	    check(snprintf(p_write, sizeof(p_write), "%d", pipes[WRITE]) < (int)sizeof(p_write),
		"snprintf truncated");
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
        int nread, proc_failures, initial_setup, i;
	int pipes[2], processes[AEROPONIC__PROCESS__MAX];
	size_t process_start_time[AEROPONIC__PROCESS__MAX];
	size_t aeroponic_start_time, process_log_count;
	char process_buff[256];

	proc_failures = 0;
	process_log_count = 0;
	initial_setup = true;
        check((processes[MONITOR] = start_monitor_child(pipes)) != -1,
	    "[FATAL] Failed to start monitor child process");

	aeroponic_start_time = (size_t)time(NULL);
	for (i = 0; i < AEROPONIC__PROCESS__MAX; i++) {
	    process_start_time[i] = (size_t)time(NULL);
	}
int x = 0;
        for ( ; ; ) {
	    check(proc_failures < PROC_FAILURE_LIMIT, "[FATAL] Processes failing, need maintenance");
	    if ((size_t)time(NULL) >= (aeroponic_start_time + SECONDS_IN_HOUR * process_log_count)) {
		log_process(((size_t)time(NULL) - aeroponic_start_time), ((size_t)time(NULL) - process_start_time[MONITOR]),
		    ((size_t)time(NULL) - process_start_time[PUMP]), ((size_t)time(NULL) - process_start_time[SOLENOID]), proc_failures);
		process_log_count++;
	    }
	    nread = read(pipes[READ], process_buff, sizeof(process_buff));
	    switch (nread) {
	    case -1:
		if (errno == EAGAIN) { /* pipe empty */
		    if (initial_setup == false && check_process_activity(processes) != 0) { /* Check if all processes are still active */
			log_err("[FATAL] Processes inactive, need aeroponic maintenance");
		    }
		    sleep(5);
		    continue;
		} else if (errno == EINVAL) {
		    log_err("[FATAL] Too much data being sent");
		} else {
		    log_err("[FATAL] read error");
		}
		break;
	    case 0:
		break;
	    default:
		debug("aeroponic receives: %s\n", process_buff);
		if (strncmp(process_buff, "ERROR:", strlen("ERROR:")) != 0) { /* solenoid or pump failed, updating processes array */
		    check(set_processes(processes, process_buff, process_start_time, &proc_failures) == 0,
			"[FATAL] Failed to set processes array");
		    initial_setup = false;
		    x++;
		} else { /* Monitor produced error Resetting processes */
		    proc_failures++;
		    check(kill_processes(processes) == 0, "[FATAL] Error killing child processes");
		    check((processes[MONITOR] = start_monitor_child(pipes)) != -1,
			"[FATAL] Failed to start monitor child process");

		    for (i = MONITOR; i < AEROPONIC__PROCESS__MAX; i++) { /* Update start times for restarting monitor process and child processes */
			process_start_time[i] = (size_t)time(NULL);
		    }
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

