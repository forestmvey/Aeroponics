#include "monitor.h"

/*
 * check processes function checks over all process passed in the pids array
 * with the process number of a dead process. When one matches the dead process that
 * index in the array is replaced with a new forked process.
 */
int
check_processes(int pids[MONITOR__PROCESS__MAX], int dead_pid)
{
        int process;

        for (process = 0; process < MONITOR__PROCESS__MAX; process++) {
            if (pids[process] == dead_pid) {
		monitor_check((pids[process] = start_child(process)) != -1, "Failed to start child process");
	    }
        }

	return 0;
error:
	return -1;
}

/*
 * Fork and exec a new process for the gpio execute solenoid operations, and return
 * the process id for that child, or -1 on error
 */
pid_t
start_child(enum process proc)
{
	pid_t pid;

	pid = fork();
	monitor_check(pid > 0, "Fork failed");
	if (pid == 0) { /* Child */
	    switch(proc) {
	    case PUMP:
		execl("./bin/pump", "./pump", (char *)NULL);
		exit(0);
		break;
	    case SOLENOID:
		execl("./bin/solenoid", "./solenoid", (char *)NULL);
		exit(0);
		break;
	    default:
		log_warn("Invalid process choice");

	    }
	} else if (pid > 0) { /* Parent */
	    return pid;
	}

error:
	return (pid_t)-1;

}

/*
 *
 */
int
main(int argc, char *argv[])
{
        int i, status, ret, write_pipe;
	char pids_string[24];
	pid_t pids[MONITOR__PROCESS__MAX];
	pid_t childpid;

	monitor_check(argc == 2, "Incorrect number of arguments sent to monitor");
	write_pipe = atoi(argv[1]);

	/*
	 * Begin process for both the solenoid and pump, and store their process id's into the pids array
	 */
	monitor_check((pids[PUMP] = start_child(PUMP)) != -1, "Failed to start pump child process");
	monitor_check((pids[SOLENOID] = start_child(SOLENOID)) != -1, "Failed to start solenoid child process");

	/*
	 * Build to process id string to send to parent process
	 */
	monitor_check(snprintf(pids_string, sizeof(pids_string), "%d %d", pids[PUMP], pids[SOLENOID]) < (int)sizeof(pids_string), "snprintf truncated");
	monitor_check(write(write_pipe, pids_string, 24) > 0, "Failed to write message to pipe");

	/*
	 * Wait for any child processes to die and restart them when they fail
	 */
//int x = 0;
        for ( ; ; ) {
            childpid = wait(&status);
//x++;
	    ret = check_processes(pids, childpid);
	    if (ret != 0) { /* Failed to start a new child process */
		/*
		 * Search to find which process failed to start and log
		 */
		for (i = 0; i < MONITOR__PROCESS__MAX; i++) {
		    if (pids[i] == childpid) {
			break;
		    }
		}
		if (i == PUMP) {
		    monitor_check(write(write_pipe, "ERROR: Failed to start process for PUMP", 48) > 0, "Failed to write message to pipe");
		} else {
		    monitor_check(write(write_pipe, "ERROR: Failed to start process for SOLONOID", 48) > 0, "Failed to write message to pipe");
		}
		exit(-1);
//if (x > 4)
//exit(0);
	    } else {
		monitor_check(snprintf(pids_string, sizeof(pids_string), "%d %d", pids[PUMP], pids[SOLENOID]) < (int)sizeof(pids_string), "snprintf truncated");
		monitor_check(write(write_pipe, pids_string, 24) > 0, "Failed to write message to pipe after process failure");
	    }
        }

	return 0;
error:
	return -1;
}
