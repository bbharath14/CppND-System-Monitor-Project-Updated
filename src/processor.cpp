#include "processor.h"
#include<iostream>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    Idle = LinuxParser::IdleJiffies();
    nonIdle = LinuxParser::ActiveJiffies();
    total = LinuxParser::Jiffies();
    long totald = total - prevTotal;
    long idled = Idle - prevIdle;
    float util = (float)(total-idled)/totald;
    prevIdle = Idle;
    prevNonIdle = nonIdle;
    prevTotal = total;
    // std::cout<<"idle:"<<Idle<<"nonidle:"<<nonIdle<<"total:"<<total;
    // std::cout<<"util="<<util;
    return util;
}
