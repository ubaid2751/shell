# Custom Shell Program

This is a simple custom shell program implemented in C++. The shell provides basic functionality for navigating directories, executing commands, and built-in commands like `echo`, `cd`, `exit`, and `type`. It also supports executing external programs by forking and using `execvp()`.

## Features

- **Built-in Commands**:
  - `echo`: Prints the arguments passed to it.
  - `cd`: Changes the current working directory.
  - `exit`: Exits the shell.
  - `type`: Checks if a command is a built-in or an external executable.
  
- **External Command Execution**: 
  - The shell can execute any program available in the `PATH` environment variable or in a specified path.
  
- **Colored Prompt**: 
  - The shell displays the current working directory in green, followed by a prompt in blue.

## How to Compile

To compile the program, use a C++ compiler like `g++`:

```bash
g++ -std=c++17 -o my_shell my_shell.cpp -lstdc++fs
```

This command compiles the C++ source file my_shell.cpp and produces an executable named my_shell. Make sure the -lstdc++fs option is included to link the C++17 filesystem library.

## How to Run

Once the program is compiled, run it by typing:

## bash

./my_shell

This will start the custom shell, where you can enter commands just like you would in a normal shell.

## Usage

    Prompt:
        The shell displays the current directory in green and waits for your input with a ~$ prompt in blue.

    Built-in Commands:
        echo [text]: Prints the text to the terminal.
        cd [directory]: Changes the current directory to the specified path.
        exit [code]: Exits the shell with an optional exit code.
        type [command]: Checks if the command is a shell built-in or an external executable.

    External Programs:
        To run external programs, simply type the name of the program (if it’s available in your system’s PATH) and press enter.
        You can also specify relative or absolute paths to executables.

Example Usage

    Change directory:

    bash

    ~$ cd /path/to/directory
    
    Echo text:
    
    bash
    
    ~$ echo Hello, World!
    
    Check if echo is a built-in command:
    
    bash
    
    ~$ type echo
    
    Run an external program:
    
    bash
    
    ~$ ls -la
    
    Exit the shell:
    
    bash
    
        ~$ exit

## Known Limitations

    No Advanced Features: This shell is a basic implementation and does not support advanced shell features such as piping (|), redirection (>, <), or background execution (&).
    Path Handling: The current version doesn’t handle relative or absolute paths without ./ or / explicitly specified by the user.
    Signal Handling: Pressing Ctrl+C (SIGINT) is not handled to interrupt a running process without terminating the shell.

## Future Improvements

    Add support for signal handling (e.g., Ctrl+C) to interrupt long-running processes without exiting the shell.
    Implement piping and redirection.
    Add support for running commands in the background using &.

## License

This project is open-source and available under the MIT License.