#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, memTotal, memFree, buffers, cached, op, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
      while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> op >> value;
      if("MemTotal:" == op){
        memTotal = value;
      }else if("MemFree:" == op){
        memFree = value;
      }else if("Buffers:" == op){
        buffers = value;
      }else if("Cached:" == op){
        cached = value;
      }
    }
  }
  long int memTotalInt = atoi(memTotal.c_str());
  long int memFreeInt = atoi(memFree.c_str());
  long int buffersInt = atoi(buffers.c_str());
  long int cachedInt = atoi(cached.c_str());
  long int memUsed = memTotalInt - memFreeInt - buffersInt - cachedInt;
  // std::cout<<"total:"<<memTotal;
  // std::cout<<"free:"<<memFree;
  // std::cout<<"buffers:"<<buffers;
  // std::cout<<"cached:"<<cached;
  return float(memUsed)/float(memTotalInt);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  std::string uptime,line;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  float uptimefloat = std::stof(uptime.c_str());
  //std::cout<<"uptimefloat="<<(long)(uptimefloat/60);
  return (long)(uptimefloat);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> states = CpuUtilization();
  long usertime1 = atoi(states[CPUStates::kUser_].c_str()) - atoi(states[CPUStates::kGuest_].c_str());
  long nicetime1 = atoi(states[CPUStates::kNice_].c_str()) - atoi(states[CPUStates::kGuestNice_].c_str());
  long idletime1 = atoi(states[CPUStates::kIdle_].c_str()) + atoi(states[CPUStates::kIOwait_].c_str());
  long systemalltime1 = atoi(states[CPUStates::kSystem_].c_str()) + atoi(states[CPUStates::kIRQ_].c_str()) + atoi(states[CPUStates::kSoftIRQ_].c_str());
  long virtualtime1 = atoi(states[CPUStates::kGuest_].c_str()) + atoi(states[CPUStates::kGuestNice_].c_str());
  long totaltime1 = usertime1 + nicetime1 + systemalltime1 + idletime1 + atoi(states[CPUStates::kSteal_].c_str()) + virtualtime1;
  return totaltime1;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
   long int utime,stime,cutime,cstime,starttime,c=0;
   std::ifstream stream(kProcDirectory + std::to_string(pid) + "/" + kStatFilename);
   std::string line,op;
   if(stream.is_open()){
     std::getline(stream,line);
     std::istringstream linestream(line);
     while(c<17){
       linestream>>op;
       //std::cout<<c<<" "<<op<<std::endl;
       if(c==13){
         utime = atoi(op.c_str());
       }else if(c==14){
         stime = atoi(op.c_str());
       }else if(c==15){
         cutime = atoi(op.c_str());
       }else if(c==16){
         cstime = atoi(op.c_str());
         break;
       }
       c++;
     }
   }
   return utime+stime+cutime+cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return Jiffies() - IdleJiffies();
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> states = CpuUtilization();
  return atoi(states[CPUStates::kIdle_].c_str()) + atoi(states[CPUStates::kIOwait_].c_str());
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string line,op;
  std::vector<string> res;
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    //to ignore cpu being added to the list
    linestream>>op;
    //std::cout<<"op:"<<op<<std::endl;
    int i=10;
    while(i>0){
      linestream>>op;
      //std::cout<<"op:"<<op<<std::endl;
      res.push_back(op);
      i--;
    }
  }
  return res;
 }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string op1,val,line;
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      if(line.find("processes")!=std::string::npos){
        linestream >> op1 >> val;
        break;
      }
    }
  }
  return atoi(val.c_str());
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::string op1,val,line;
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      if(line.find("running")!=std::string::npos && line.find("procs")!=std::string::npos){
        linestream >> op1 >> val;
        break;
      }
    }
  }
  return atoi(val.c_str());
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + "/" + kCmdlineFilename);
    std::string cmd;
    if(stream.is_open()){
        std::getline(stream, cmd);
    }
  return cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
   std::ifstream stream(kProcDirectory + std::to_string(pid) + "/" + kStatusFilename);
   std::string op,val, line;
   if(stream.is_open()){
     while(std::getline(stream, line)){
       if(line.find("VmSize:")!=std::string::npos){
          std::istringstream linestream(line);
          linestream >> op >> val;
          break;
        }
     }
   }
   return std::to_string(atoi(val.c_str())/1000);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + "/" + kStatFilename);
  std::string op,val,line,res;
  if (stream.is_open()) {
    while(std::getline(stream, line)){
        if(line.find("Uid:")!=std::string::npos){
            std::istringstream linestream(line);
            linestream>>op>>val;
        }
    }
  }
  return val;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
   std::ifstream stream1(kPasswordPath);
   std::string line, res;
  if(stream1.is_open()){
      while(std::getline(stream1, line)){
          if(line.find(Uid(pid) +":"+ Uid(pid))!=std::string::npos){
              std::replace(line.begin(), line.end(), ':', ' ');
              std::istringstream linestream1(line);
              linestream1>>res;
              break;
          }
      }
  }
  return res;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
   std::ifstream stream(kProcDirectory + std::to_string(pid) + "/" + kStatFilename);
    int c=0;
    std::string line,word;
    if(stream.is_open()){
        std::getline(stream,line);
        std::istringstream linestream(line);
        while(c!=22){
            linestream >> word;
            c++;
        }
    }
    return atoi(word.c_str())/sysconf(_SC_CLK_TCK);
}