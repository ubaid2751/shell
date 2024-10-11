#include<iostream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<sstream>
#include<filesystem>
#include<unistd.h>
#include<sys/wait.h>
#include<limits.h>
using namespace std;

string builtins_cmds[] = {"echo", "exit", "type", "cd"};

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

    string command_path = get_path(command[0]);
    if(!command_path.empty()) {
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

            if(execvp(command_path.c_str(), args.data()) < 0) {
                cerr << "Failed to execute command\n";
                return 0;
            }
        }
        else {
            int status;
            waitpid(pid, &status, 0);
            return 1;
        }
    }

    return 0;
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

int main() {
    string command;
    bool exit = 0;

    while(!exit) {
        cout << unitbuf;
        cerr << unitbuf;

        cout << currdir() << ":";
        cout << "$ ";
        getline(cin, command);

        vector<string> args = parse_command(command);
        if (!args.empty()) {
            if (!handle_command(args)) {
                cout << command << ": command not found\n";
            }
        }
    }
}