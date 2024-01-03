#include "systemcalls.h"
#include <fcntl.h>     // open()
#include <stdlib.h>    // system()
#include <sys/stat.h>
#include <sys/wait.h>  // waitpid()
#include <unistd.h>    // fork()

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd) {
    return system(cmd) == 0;
}

bool _do_exec_v(const char *outputfile, int count, va_list args) {
    
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    const pid_t pid = fork();
    if (pid == -1) {
        perror("fork () failed");
        return false;
    } 
    
    if (pid == 0) { 
        // Child process.

        // First, re-direct stdout if needed.
        if (outputfile) {
            // https://stackoverflow.com/a/13784315/1446624
            const int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT,
                S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);


            // dup2 (int fd, int fd2) - duplicates FD to FD2, closing FD2 and making it open on the same file,
            // so the below will close fd 1 (stdout) first, and then make fd 1 a "clone" of our fd.
            if (dup2(fd, 1) < 0) {
                perror("dup2() failed");
            }
            // we can now close the "original" fd.
            close(fd);
        }

        // int execv(const char *pathname, char *const argv[])
        // The initial argument (argv[0]) is the name of a file that is to be executed.
        execv(/* pathname */ command[0], /* argv[] */ &command[0]);

        // exec() functions return only if an error has occurred.
        perror("execv() failed");
        exit(EXIT_FAILURE);
    } else { // Parent process.
        int status;
        if (waitpid(pid, &status, 0) != pid) {
            perror("waitpid() failed");
            return false;
        }

        // Check the exit status.
        return WEXITSTATUS(status) == 0;
    }
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...) {
    va_list args;
    va_start(args, count);
    bool res = _do_exec_v( /* stdout redirect */ NULL, count, args);
    va_end(args);
    return res;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...) {
    va_list args;
    va_start(args, count);
    bool res = _do_exec_v(outputfile, count, args);
    va_end(args);
    return res;
}
