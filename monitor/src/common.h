#ifndef _common_h
#define _common_h

#define READ 0
#define WRITE 1

#define PIPES_BUFF_SIZE 5

enum process {
	PUMP,
	SOLONOID,
	MONITOR,
        PROCESS__MAX
};

/*
 * Print the process associated with the enumeration
 */
char*
get_process_string(int process)
{
        switch(process) {
        case (PUMP):
            return "PUMP";
        case (SOLONOID):
            return "SOLONOID";
        case (MONITOR):
            return "MONITOR";
        default:
            return "INVALID PROCESS PARAMETER";
        }
}

#endif
