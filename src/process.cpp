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


int Process::Pid() { return pid; }


float Process::CpuUtilization() { 
    return (float)(float(LinuxParser::ActiveJiffies(this->pid))/(float(sysconf(_SC_CLK_TCK))))/float(this->UpTime());
}


string Process::Command() { 
    return LinuxParser::Command(this->pid);
}


string Process::Ram() {
    return LinuxParser::Ram(this->pid);
}


string Process::User() { 
  return LinuxParser::User(this->pid);
}


long int Process::UpTime() {
    this->uptime = LinuxParser::UpTime(this->pid);
    return this->uptime;
}

// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    if(this->uptime<a.uptime)
        return true;
    return false;
}