#include<iostream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<sstream>
#include<filesystem>
#include<unistd.h>
#include<sys/wait.h>
#include<limits.h>
#include<csignal>
#include<termios.h>
using namespace std;

#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"

#define PRINT_PROMPT(input) do { \
    cout << "\r" << GREEN << BOLD << currdir() << RESET << ":"; \
    cout << BLUE << BOLD << "~$ " << RESET << "\033[K" << input; \
    cout.flush(); \
} while (0)

#define MAIN_FILE "#include<iostream>\n\nusing namespace std;\n\nint main() {\n    cout << \"Hello, World!\" << endl;\n    return 0;\n}\n"

string builtins_cmds[] = {"echo", "exit", "type", "cd"};

vector<string> command_history;
int history_index = -1;

string get_path(string command) {
    string path_env = getenv("PATH");
    stringstream ss(path_env);
    string path;

    while(!ss.eof()) {
        getline(ss, path, ':');

        string abs_path = path + '/' + command;

        if(filesystem::exists(abs_path)) {
            return abs_path;
        }
    }

    return "";
}

vector<string> parse_command(string command) {
    vector<string> args;
    stringstream ss(command);
    string arg = "";

    while(ss >> arg) {
        args.push_back(arg);
    }

    return args;
}

void echo_command(vector<string> &command) {
    for(int i = 1; i < command.size(); i++) {
        cout << command[i] << " ";
    }
    cout << endl;
}

bool search(string cmd) {
    int num_builtins = sizeof(builtins_cmds) / sizeof(builtins_cmds[0]);
    for(int i = 0; i < num_builtins; i++) {
        if(builtins_cmds[i] == cmd) {
            return true;
        }
    }
    return false;
}

string currdir() {
    char cwd[PATH_MAX];

    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        return (string)cwd;
    } else {
        cerr << "Failed_to_recognise_directory\n";
    }

    return "";
}

void remove_Project_Folder(string foldername) {
    string path = currdir() + "/" + foldername;
    filesystem::remove_all(path);
}

bool handle_command(vector<string> &command) {
    if(command[0] == "exit") {
        if(command.size() == 2) {
            if(command[1] == "0") {
                exit(0);
            } else {
                cout << "exit: " << command[1] << ": numeric argument required\n";
            }
        } else {
            exit(0);
        }
    }

    if(command[0] == "echo") {
        echo_command(command);
        return 1;
    }

    if(command[0] == "type") {
        if (command.size() == 2) {
            if(search(command[1])) {
                cout << command[1] << " is a shell builtin\n";
            } else {
                string path = get_path(command[1]);
                if(path.empty()) {
                    cout << command[1] << " not found\n";
                }
                else {
                    cout << command[1] << " is " << path << endl;
                }
            }
            return 1;
        }
    }

    if(command[0] == "cd") {
        if(command.size() == 1) {
            chdir(getenv("HOME"));
            return 1;
        }
        if(command.size() == 2) {
            if(chdir(command[1].c_str()) == -1) {
                cout << "cd: " << command[1] << ": No such file or directory\n";
            }
            return 1;
        }
    }

    if(command[0] == "generate" && command[1] == "project") {
        vector<const char*> foldnames = {"src", "include", "bin", "lib", "docs"};
        vector<const char*> filenames = {".gitignore", "build.sh", "docs/README.md", "src/main.cpp"};
        if(command.size() != 3) {
            cout << "Usage: generate project <foldername>\n";
            return 1;
        }
        else {
            string foldername = command[2];
            string path = currdir() + "/" + foldername;
            if(filesystem::exists(path)) {
                cout << "genProFolder: " << foldername << ": File exists\n";
                return 1;
            }
            if(filesystem::create_directory(path)) {
                cout << "genProFolder: " << foldername << " created\n";

                for (const auto& folder : foldnames) {
                    string fullpath = path + "/" + folder;
                    if (!filesystem::create_directory(fullpath)) {
                        std::cerr << "Failed to create directory: " << fullpath << "\n";
                        remove_Project_Folder(foldername);
                    }
                }

                for (const auto& file : filenames) {
                    std::string fullpath = path + "/" + file;
                    FILE* f = fopen(fullpath.c_str(), "w");

                    if (f == NULL) {
                        std::cerr << "Failed to create file: " << fullpath << "\n";
                        remove_Project_Folder(foldername);
                    } else {
                        if(file == "src/main.cpp") {
                            fprintf(f, "%s", MAIN_FILE);
                        }
                        fclose(f);
                    }
                }

                cout << "genProFolder: " << foldername << ": created\n";
                cout << "src/main.cpp\n docs/README.md\n";
                return 1;
            }
            else {
                cout << "genProFolder: " << foldername << ": Failed to create folder\n";
                return 1;
            }
        }
    }

    pid_t pid = fork();
    if(pid == -1) {
        cerr << "Failed to fork\n";
        return 0;
    }
    else if(pid == 0) {
        vector<char*> args;
        for(int i = 0; i < command.size(); i++) {
            args.push_back(&command[i][0]);
        }
        args.push_back(nullptr);
        if(execvp(command[0].c_str(), args.data()) < 0) {
            cerr << "Failed to execute command\n";
            exit(EXIT_FAILURE);
        }
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        return 1;
    }

    return 0;
}

void sigint_handler(int sig) {
    if(sig == 2) {
        cout << "^C\n";
        cout << GREEN << BOLD << currdir() << RESET << ":";
        cout << BLUE << BOLD << "~$ " << RESET;
        cout.flush();
    }
}

void set_raw_mode(termios &old_term) {
    termios new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
}

void restore_terminal(const termios &old_term) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
}

string read_command_with_history() {
    termios old_term;
    set_raw_mode(old_term);
    
    string input = "";
    char ch;
    
    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 127) {
            if (!input.empty()) {
                input.pop_back();
                cout << "\b \b";
            }
        } else if (ch == '\n') {
            cout << endl;
            break;
        } else if (ch == '\033') {
            char seq[2];
            if (read(STDIN_FILENO, seq, 2) == 2) {
                if (seq[0] == '[') {
                    if (seq[1] == 'A') {
                        if (history_index > 0) {
                            history_index--;
                            input = command_history[history_index];
                            PRINT_PROMPT(input);
                        }
                    } else if (seq[1] == 'B') {
                        if (history_index < command_history.size() - 1) {
                            history_index++;
                            input = command_history[history_index];
                            PRINT_PROMPT(input);
                        } else {
                            history_index = command_history.size();
                            input.clear();
                            PRINT_PROMPT(input);
                        }
                    }
                }
            }
        } else {
            input += ch;
            cout << ch;
        }
    }
    
    restore_terminal(old_term);
    
    return input;
}


int main() {
    string command;
    bool exit = 0;

    signal(SIGINT, sigint_handler);

    while(!exit) {
        cout << unitbuf;
        cerr << unitbuf;

        cout << GREEN << BOLD << currdir() << RESET << ":";
        cout << BLUE << BOLD << "~$ " << RESET;

        command = read_command_with_history();
        if (!command.empty()) {
            command_history.push_back(command);  // Add command to history
            history_index = command_history.size();  // Reset history index
            vector<string> args = parse_command(command);
            if (!args.empty()) {
                if (!handle_command(args)) {
                    cout << command << ": command not found\n";
                }
            }
        }
    }
}
