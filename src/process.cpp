#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  vector<string> cpu_utilization = LinuxParser::CpuUtilization(pid_);
  float uptime     = float(LinuxParser::UpTime());
  float utime      = std::stof(cpu_utilization[13]);
  float stime      = std::stof(cpu_utilization[14]);
  float cutime     = std::stof(cpu_utilization[15]);
  float cstime     = std::stof(cpu_utilization[16]);
  float starttime  = std::stof(cpu_utilization[21]);
  float cpu_usage{0.0};
  
  float total_time = (utime + stime + cutime + cstime)/sysconf(_SC_CLK_TCK);
  float seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
  
  cpu_usage = total_time / seconds;
  
  return cpu_usage; 
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_).substr(0,20);}

// TODO: Return this process's memory utilization
string Process::Ram() { 
  string ram_KB = LinuxParser::Ram(pid_);
  int ram_MB = std::stoi(ram_KB)/1024;
  return to_string(ram_MB);}

// TODO: Return the user (name) that generated this process
string Process::User() { return  LinuxParser::User(pid_);}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_);}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
// bool Process::operator<(Process const& a) const { 
//   if (this->CpuUtilization() > a.CpuUtilization()) {return true;}
//   return false; 
// }