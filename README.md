# Shell

**Assignment 2 - Operating Systems And Networks, Monsoon 2022**

The working directory of the shell at startup, that is the directory where it was executed from, is considered to be the home.

Commands can be chained with `;` or `&`. `;` will run the previous command in foreground, whereas `&` will run it in background.

#### Inbuild Commands

- **echo:** `<space-tab-separated-strings>` prints the strings as it as, while ignring the spacing.
- **cd:** `-, ., .., <relative-path>, <absolute-path>`
- **pwd:** prints the current working directory. Takes no arguements.
- **ls**: `-a, -l, <path(s)>`

#### More info

##### ls

- Flags: `a, l`
    - `a`: display hidden files
    - `l`: display extra info

Accepts paths to both directories and files. For directories it will list all the files and directories inside it.
