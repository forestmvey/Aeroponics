#include "aeroponic.h"

/*
 * Set the process array with the process id's for given child processes. In the case
 * of update after a process has failed, log the given process uptime before failure.
 */
int
set_processes(int* process, char* str, size_t* process_start_time, int* proc_failures)
{
	char* proc = NULL;

	proc = strtok(str, " ");
	for (int i = 0; i < MONITOR__PROCESS__MAX && proc != NULL; i++) {
	    if (process[i] != atoi(proc) && process[i] != 0) { /* Process failure */
		log_info("Process %s failed after %zu uptime", get_process_string(i), ((size_t)time(NULL) - process_start_time[i]));
		process_start_time[i] = (size_t)time(NULL);
		proc_failures++;
	    }
	    process[i] = atoi(proc);
	    proc = strtok(NULL, " ");
	}
	check(clear_gpio() == 0, "Failed to clear gpio");

	return 0;
error:
	return -1;
}

/*
 * Kill all child processes and clear gpio pins.
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
 * Check if any processes are showing operation errors.
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

/*
 * Begin a child process for the monitor binary with non-blocking communication pipes. Only one way
 * communication is needed from monitor -> aeroponic.
 */
pid_t
start_monitor_child(int pipes[])
{
        char p_write[10];
        pid_t pid;

        check(pipe(pipes) >= 0, "Pipe failed");

	check(fcntl(pipes[READ], F_SETFL, O_NONBLOCK) >= 0, "fcntl failed for pipe"); /* set pipe non-blocking */

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

/*
 * Aeroponic binary utility is to operate GPIO pins 7 and 12 on a raspberry pi model 3B+ to deliver power to
 * a water pump and solenoid in order to operate a high pressure aeroponic farming system. Aeroponic spawns one child
 * process monitor, which in turn spawns 2 child processes pump and solenoid. Pump and solenoid manage timing the gpio pin output for voltage
 * to a relay turning on the solenoid and pump. Aeroponic monitors all processes activity by their process id's. If a solenoid or pump process
 * dies the monitor process sends an updated set of process id's to aeroponic. Aeroponic updates its process tracking array and logs the failure.
 * in the case of operations errors on monitor, an error message will be sent to aeroponic and logged. If the monitor process fails aeroponic will
 * find the failure when checking the process id's status, and will subsequently log the error and reset the gpio and monitor process. If the error
 * limit set in common.h, is reached, a fatal error will be logged requiring maintenance, and operations will halt.
 */
int
main(void)
{
        int nread, proc_failures, initial_setup, i;
	int pipes[2], processes[AEROPONIC__PROCESS__MAX];
	size_t process_start_time[AEROPONIC__PROCESS__MAX];
	size_t aeroponic_start_time, process_log_count;
	char process_buff[256];

	proc_failures = 0;
	process_log_count = 0;
	initial_setup = true;

	/*
	 * Set all start times for processes for logging use.
	 */
	aeroponic_start_time = (size_t)time(NULL);
	for (i = 0; i < AEROPONIC__PROCESS__MAX; i++) {
	    process_start_time[i] = (size_t)time(NULL);
	}

        check((processes[MONITOR] = start_monitor_child(pipes)) != -1,
	    "[FATAL] Failed to start monitor child process");


        for ( ; ; ) {
	    check(proc_failures < PROC_FAILURE_LIMIT, "[FATAL] Processes failing, need maintenance");

	    /*
	     * Every hour log process uptimes and a process failures count
	     */
	    if ((size_t)time(NULL) >= (aeroponic_start_time + SECONDS_IN_HOUR * process_log_count)) {
		log_process(((size_t)time(NULL) - aeroponic_start_time), ((size_t)time(NULL) - process_start_time[MONITOR]),
		    ((size_t)time(NULL) - process_start_time[PUMP]), ((size_t)time(NULL) - process_start_time[SOLENOID]), proc_failures);
		process_log_count++;
	    }

	    /*
	     * Read from pipe to check for any failed processes or operational errors
	     */
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
	    }
        }

error:
	kill_processes(processes);
        exit(-1);
}

