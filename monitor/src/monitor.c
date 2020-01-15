#include "monitor.h"

/*
 * check processes function checks over all process passed in the pids array
 * with the process number of a dead process. When one matches the dead process that
 * index in the array is replaced with a new forked process.
 */
int
check_processes(int pids[PROCESS__MAX], int dead_pid)
{
        int process;
        pid_t pid;

        for (process = 0; process < PROCESS__MAX; process++) {
            if (pids[process] == dead_pid) {
		log_info("process for exchange %s failed", get_process_string(process));
                pid = fork();
                if (pid < 0) {
                    log_err("Fork failed");
                }
                if (pid == 0) { /* child process  */
		    if (process == PUMP) {
	    	        execl("./bin/pump", "./pump", NULL);
	    	        exit(0);
		    } else {
	    	        execl("./bin/solonoid", "./solonoid", NULL);
	    	        exit(0);
		    }
                }
		if (pid > 0) { /* parent process  */
                    pids[process] = pid;
		}
            }
        }

	return 0;
error:
	return -1;
}

/*
 * Fork and exec a new process for the gpio execute solonoid operations, and return
 * the process id for that child, or -1 on error
 */
pid_t
start_solonoid_child()
{
	pid_t pid;

	pid = fork();
	if (pid < 0) {
	    log_err("Fork failed");
	} else if (pid == 0) { /* Child */
	    execl("./bin/solonoid", "./solonoid", NULL);
	    exit(0);
	} else if (pid > 0) { /* Parent */
	    return pid;
	}

error:
	return (pid_t)-1;

}

/*
 * Fork and exec a new process for the gpio execute pump operations, and return
 * the process id for that child, or -1 on error
 */
pid_t
start_pump_child()
{
	pid_t pid;

	pid = fork();
	if (pid < 0) {
	    log_err("Fork failed");
	} else if (pid == 0) { /* Child */
	    execl("./bin/pump", "./pump", NULL);
	    exit(0);
	} else if (pid > 0) { /* Parent */
	    return pid;
	}

error:
	return (pid_t)-1;

}

int
main(int argc, char *argv[])
{
        int i, status, ret, write_pipe;
	pid_t pids[PROCESS__MAX];
	pid_t childpid;

	printf("arg 1: %s argc: %d\n", argv[0], argc);
	check(argc == 2, "Incorrect number of arguments sent to monitor");
	write_pipe = atoi(argv[1]);
exit(0);
	/*
	 * Begin process for both the solonoid and pump, and store their process id's into the pids array
	 */
	pids[PUMP] = start_pump_child();
	check(pids[PUMP] != -1, "Failed to start child");
	pids[SOLONOID] = start_solonoid_child();
	check(pids[SOLONOID] != -1, "Failed to start child");

	/*
	 * Wait for any child processes to die and restart them when they fail
	 */
        for ( ; ; ) {
            childpid = wait(&status);
            check(write(write_pipe, "child crashed. \n", 256) > 0, "Failed to write message to pipe");
log_err("monitor finished exiting now");
	    ret = check_processes(pids, childpid);
	    if (ret != 0) {
		/*
		 * Search to find which process failed to logging
		 */
		for (i = 0; i < PROCESS__MAX; i++) {
		    if (pids[i] == childpid) {
			break;
		    }
		}
		log_info("Failed to start curl child process for exchange: %s", get_process_string(i));
	    }
        }

	/*
	 * Kill all child processes before exiting
	 */
	kill(pids[0], SIGKILL);
	kill(pids[1], SIGKILL);
	return 0;
error:
	kill(pids[0], SIGKILL);
	kill(pids[1], SIGKILL);
	return -1;
}
