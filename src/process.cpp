#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include<algorithm>
#include<iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    //return 0;
    return (float)(float(LinuxParser::ActiveJiffies(this->pid))/(float(sysconf(_SC_CLK_TCK))))/float(this->UpTime());
}

// TODO: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(this->pid);
}

// TODO: Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(this->pid);
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
  return LinuxParser::User(this->pid);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    this->uptime = LinuxParser::UpTime(this->pid);
    return this->uptime;
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    if(this->uptime<a.uptime)
        return true;;
    return false;
}