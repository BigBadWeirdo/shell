#include <linux/limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include "shell.hpp"

using namespace std;

static void cd(char** args)
{
    if (!args[1]) {
        cout << "goto home not implemented yet!" << endl;
    } else {
        if (chdir(args[1]) < 0) {
            perror("chdir");
        }
    }
}

void Shell::init()
{
    m_builtins["cd"] = cd;
}

/*
 * Main shell loop
 */
void Shell::run()
{
    cout << "**********************" << endl;
    cout << "Welcome to Krist Shell" << endl;
    cout << "**********************" << endl;

    init();

    int status;
    string line;
    char wd[PATH_MAX];

    do {
        getcwd(wd, PATH_MAX);
        cout << wd << "$ ";
        if (!getline(cin, line)) {
            break;
        }
        vector<string> args = parse_line(line);
        status = launch(args);
    } while (status);

    cout << "bye!" << endl;
}

vector<string> Shell::parse_line(const string& line)
{
    vector<string> args;
    boost::char_separator<char> sep(" ", "|&");
    boost::tokenizer<boost::char_separator<char>> tok(line, sep);
    for (auto it = tok.begin(); it != tok.end(); ++it) {
        args.push_back(*it);
    }
    return args;
}

int Shell::launch(const vector<string>& args)
{
    if (args.empty()) {
        return 1;
    }

    vector<char*> cargs;
    for (auto& arg : args) {
        cargs.push_back(const_cast<char*>(arg.c_str()));
    }
    cargs.push_back(nullptr);

    if (m_builtins.find(args[0]) != m_builtins.end()) {
        m_builtins[args[0]](&cargs[0]);
        return 1;
    }

    pid_t pid = fork();  // NOT a syscall !   calls clone... (which is a syscall)
    if (pid == 0) {
        // child
        if (execvp(cargs[0], &cargs[0]) < 0) {  // not a syscall...  (but execve is )
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        // parent
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
