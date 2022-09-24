# Shell

**Assignment 2/3 - Operating Systems And Networks, Monsoon 2022**

### Building

Tested on `fedora 35` running on `kernel 5.19.4`. Tested compilation on `gnu make 4.3` and `gcc 11.3.1`. Run `make all` in the root directory to build. Run `./batak` to start the shell.

### Navigating the source

The `src` folder contains all the source code of the project. The source is mainly divided into three directory:

- `src` Contains the main file, and utilities required by components all over the project.
- `src/core` Contains the core components of the shell, like `execute.c` which is executes commands, foreground and background processes.
- `src/builtins` Contains the builtin commands.

#### Files

| File | Description    | File    | Description    |
|---------------- | --------------- | --------------- | --------------- |
| `main.c`    | contains the `main` function and signal handling code    | `utils.(c/h)`    | contains utility functions    |
| `logger.(c/h)`    | contains utilities for info and error logging   | `globals.h`   | contains global macros and variables, defined by extern   |
| | | | |
| `builtins/builtins.h`    | declares all the builtin command and functions for initialising them    | `builtins/builtins.c`    | defines functions for initialising builtin commands    |
| `builtins/*.c`    | contains definitions of builtin commands    |     |     |
| | | | |
| `core/execute.(c/h)`    | declares and defines functions for executing commands    |  `core/history.(c/h)`   |  contains code for loading, saving and recording commands in history   |
| `core/parse.(c/h)`    | declares and defines functions for parsing commands, then executes them using functions declared in `execute.h`    |  `core/process_list.(c/h)`   |  defines a linked list for storing all the background processes started by the shell   |
| `core/prompt.(c/h)` | declares and defines function for printing the prompt, uses state variables from `globals.h` to show last command status and time | `core/io.(c/h)` | defines a `print` function which prints to stdout and keeps track of number of lines printed, written for fixing the double prompt bug |
| `core/autocomplete.(c/h)` | contains code for handling autocompletion and displaying the possible completions | | |


### Usage

The working directory of the shell at startup, that is the directory where it was executed from, is considered to be the home.

Commands can be chained with `;` or `&`. `;` will run the previous command in foreground, whereas `&` will run it in background.

`tab` can be pressed to trigger autocompletion.

#### Pipe And Redirection

Processes can have their input output redirected to files or to another process. You can pipe output of one process to another by using `|` and it can be chained any number of times.

You can take input from a file for the first process in a pipeline and redirect the last processes output to a file.

- `cat file.txt | wc`
- `ls | cat | wc`
- `cat < file.txt | wc > out.txt`

**Note:** For now pipelines cannot run in background. They can be suspended and continued but they cannot run in background. Attempting to do so will result in a parsing error.

#### Inbuilt Commands

- **exit**: exits the shell
- **echo:** `<space-tab-separated-strings>` prints the strings as it as, while ignring the spacing.
- **cd:** `-, ., .., <relative-path>, <absolute-path>`
- **pwd:** prints the current working directory. Takes no arguements.
- **ls**: `-a, -l, <path(s)>`
- **pinfo**: `<optional-pid>`
- **history**: prints the last 10 commands
- **discover**: `<target-dir> -d -f <filter>` Search for files/directories recursively in the given directory
- **jobs**: `-r, -s` prints all stopped or running background jobs started by the shell
- **sig**: `<index> <signal>` raises `<signal` signal for the job with index `<index>` in jobs list
- **fg**: `<index>` brings the job with index `<index>` to the foreground and continues it if it is stopped
- **bg**: `<index>` continues a background job with index `index` if it is stopped

#### More info

##### cd : change directory

- Flags: `-`
    - `-`: Goes to previous directory and prints its path. By default the directory where the shell was executed from is considered to be the previous directory.

##### ls : list files/directories

- Flags: `a, l`
    - `a`: display hidden files
    - `l`: display extra info

Accepts paths to both directories and files. For directories it will list all the files and directories inside it.

##### pinfo : process information

Gives information like status and virtual memory size for the current process or process with the given PID.

> Sample Output
```bash
pid : 205888
process status : R+
memory : 1980
executable path : ~/batak
```

##### history : command history

Records commands in a file and saved across sessions. Currently, the values of how many commands to store and display are fixed.

##### discover : find files

- Flags: `d, f`
    - `d`: search for directories
    - `f`: search for files
- Example:
    - `discover -d src "build"` this will output all folders named in `build` inside `src`
    - `discover -f src ".c"` this will output all files with extension `.c` inside `src`
    - `discover -d -f src "tmp"` this will output all folders/files with named `tmp` inside `src`
    - `discover -d -f ~ ` thi will output all files/folders in `~`
    - `discover ~ ` thi will output all files/folders in `~`

Lists all files/folders(depending on flags) matching the filter, in the given target directory. For now `<filter>` just accepts normal strings.
