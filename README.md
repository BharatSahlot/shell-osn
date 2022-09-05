# Shell

**Assignment 2 - Operating Systems And Networks, Monsoon 2022**

The working directory of the shell at startup, that is the directory where it was executed from, is considered to be the home.

Commands can be chained with `;` or `&`. `;` will run the previous command in foreground, whereas `&` will run it in background.

#### Inbuilt Commands

- **exit**: exits the shell
- **echo:** `<space-tab-separated-strings>` prints the strings as it as, while ignring the spacing.
- **cd:** `-, ., .., <relative-path>, <absolute-path>`
- **pwd:** prints the current working directory. Takes no arguements.
- **ls**: `-a, -l, <path(s)>`
- **pinfo**: `<optional-pid>`
- **history**: prints the last 10 commands
- **discover**: `<target-dir> -d -f <filter>` Search for files/directories recursively in the given directory

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

##### history : command history

Records commands in a file and saved across sessions. Currently, the values of how many commands to store and display are fixed.

##### discover : find files

- Flags: `d, f`
    - `d`: search for directories
    - `f`: search for files

Lists all files/folders(depending on flags) matching the filter, in the given target directory. For now `<filter>` just accepts normal strings.
