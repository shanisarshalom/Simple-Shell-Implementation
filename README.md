# Custom Shell Implementation in C

## Overview

`myshell.c` is a simple, custom shell program implemented in C. It handles basic Linux commands such as `ls`, `cat`, and `sleep`, while also supporting additional shell-specific functionality like `history`, `cd`, `pwd`, and `exit`. The shell uses `fork()` and `exec()` system calls to execute commands in the foreground and ensures robust error handling.

---

## Objectives

- Develop a custom shell in C capable of executing basic Linux commands using system calls (`fork()`, `exec()`).
- Implement specific internal commands (`history`, `cd`, `pwd`, `exit`) without relying on Unix default implementations.
- Ensure proper command execution in the foreground with error detection and appropriate messaging.

---

## Features

1. **Support for Basic Commands**:  
   - Executes standard Linux commands like `ls`, `cat`, `sleep`, and others available in the directories specified by `$PATH`.

2. **Custom Commands**:  
   - **`history`**: Maintains and displays a record of the last 100 commands executed.  
   - **`cd`**: Changes the current working directory.  
   - **`pwd`**: Displays the current working directory.  
   - **`exit`**: Exits the shell cleanly.  

3. **Environment Variable Support**:  
   - Recognizes executables located in directories specified by the `$PATH` variable or additional directories provided as arguments when launching the shell.  
   - Ensures the state of environment variables remains unchanged after the shell exits.

4. **Command Limitations**:  
   - Handles up to **100 commands**, with a maximum command length of **100 characters**.  
   - Assumes no spaces in directory or file names passed as arguments.

---

## Assumptions

- Commands and paths passed to the shell do not include spaces.  
- The shell will be used interactively in a standard Linux environment.  
- Input is limited to basic ASCII characters.  

