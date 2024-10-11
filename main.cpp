#include<iostream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<sstream>
#include<filesystem>
using namespace std;

string builtins_cmds[] = {"echo", "exit", "type"};

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
    string arg = "";
    for(int i = 0; i < command.size(); i++) {
        if(command[i] == ' ') {
            args.push_back(arg);
            arg = "";
        } else {
            arg += command[i];
        }
    }

    args.push_back(arg);
    return args;
}

void echo_command(vector<string> &command) {
    for(int i = 1; i < command.size(); i++) {
        cout << command[i] << " ";
    }
    cout << endl;
}

bool search(string cmd) {
    for(int i = 0; i < builtins_cmds->size(); i++) {
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

    return 0;
}

int main() {
    string command;
    bool exit = 0;

    while(!exit) {
        cout << unitbuf;
        cerr << unitbuf;
    
        cout << "$ ";
        getline(cin, command);

        vector<string> args = parse_command(command);
        if(handle_command(args)) {
            continue;
        }
        else {
            cout << command << ": command not found\n";
        }
    }
}