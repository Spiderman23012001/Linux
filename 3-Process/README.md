# PROCESS

## Introduction
- A program is a group of instructions that perform a specific task, represented by executable files and stored on a computer's hard drive.
- A process is a program that is being executed and utilizing the system's resources.
## Command Line Arguments
- argc: The number of parameters passed to the main() function.
- argv[]: An array of pointers pointing to the parameters passed to the program.

## Memory layout
- The memory layout of a program is divided into several sections:
    - Code segment: Stores the program's instructions.
    - Data segment: Stores initialized and uninitialized variables.
    - Stack segment: Stores local variables and function call information.
    - Heap segment: Stores dynamically allocated memory.
    - BSS segment: Stores uninitialized variables.
    - Text segment: Stores the program's code.

## Process Manipulation
- Process creation: Using fork() or clone()
- If you want run program B in process A, you can use fork() and then exec() to run program B.
- Wait for a process: Using wait()
- Normally Termination: Using exit() or _exit()
- Abnormal Termination: Using abort() or raise()
- Process termination: Using kill() or killpg()

## Process Management
- wait() is used to wait for the termination of a child process. Returns the process ID of the child process that terminated.
- waitpid(): Waits for the termination of a child process. Returns the process ID of the child process that terminated.
- If one process terminated , the other process will continue to run.
## Zombie Process
- Zombie process is a process that has finished execution but still has an entry in the process table.
-> Preventing zombie processes using SIGCHLD, wait(), waitpid()
## Orphane Process
- Orphan process is a process that has finished execution but still has an entry in the process table and its parent process has finished execution.
- Preventing orphan processes using wait(), waitpid()
