#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <boost/function.hpp>

using namespace std;

class Shell
{
public:
    void run();

private:
    vector<string> parse_line(const string& line);
    int launch(const vector<string>& args);
    void init();

    unordered_map<string, boost::function<void (char**)>> m_builtins;
};
