#include "processor.h"
#include<iostream>


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
    return util;
}
