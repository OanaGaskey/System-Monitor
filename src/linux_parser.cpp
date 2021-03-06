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

//define helper function that opens proc folder and finds given key associated value in given file
template <typename T>
T LinuxParser::GetValueByKey(const string& file, const string& key){
  string line;
  string stream_key;
  T value;
  
  std::ifstream filestream(kProcDirectory+file);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> stream_key >> value;
      if (stream_key == key) { 
        filestream.close();
        return value; 
      }
    }
    filestream.close();
  }
  return value;
}

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
          filestream.close();
          return value;
        }
      }
    }
    filestream.close();
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return version+" "+kernel;
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
  string mem_total = "MemTotal:";
  string mem_free = "MemFree:";
  float Total = GetValueByKey<float>(kMeminfoFilename, mem_total);
  float Free = GetValueByKey<float>(kMeminfoFilename, mem_free);
  return (Total - Free) / Total;
}

// TODO: Read and return the system uptime
long int LinuxParser::UpTime() { 
  string line;
  string value;
  long int up_time {1};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    up_time = std::stol(value);
  }
  filestream.close();
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long total_jiffies{0};
  total_jiffies = ActiveJiffies() + IdleJiffies();
  return total_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
//long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long active_jiffies{0};
  vector<string> cpu_utilization = CpuUtilization();

  active_jiffies = std::stol(cpu_utilization[kUser_]) + 
    			   std::stol(cpu_utilization[kNice_]) +
       			   std::stol(cpu_utilization[kSystem_]) +
       			   std::stol(cpu_utilization[kIRQ_]) +
       			   std::stol(cpu_utilization[kSoftIRQ_]) +
        		   std::stol(cpu_utilization[kSteal_]);
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idle_jiffies{0};
  vector<string> cpu_utilization = CpuUtilization();
  idle_jiffies = std::stol(cpu_utilization[kIdle_]) + std::stol(cpu_utilization[kIOwait_]);
  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string jiffie;
  vector<string> jiffies{};
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    //stream the first string "cpu" and do nothing with it
    linestream >> jiffie;
    //stream the rest of the line until its end
    while(linestream >> jiffie){ jiffies.push_back(jiffie); }
//     std::cout<<"CPU file read with jiffies: "<<jiffies.size()<<std::endl;
//     for (auto s : jiffies){std::cout<<s<<std::endl;}
  }
  filestream.close();
  return jiffies;
}

//CPU utilization for a specific process
vector<string> LinuxParser::CpuUtilization(int pid) { 
  string line;
  string jiffie;
  vector<string> jiffies{};
  
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    
    //stream the the line until its end
    while(linestream >> jiffie){ jiffies.push_back(jiffie); }
  }
  filestream.close();
  return jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return GetValueByKey<int>(kStatFilename, "processes"); }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return GetValueByKey<int>(kStatFilename, "procs_running"); }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line{};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()) {std::getline(filestream, line); }
  filestream.close();
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {  
  string line;
  string key;
  string value{};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") {return value;}
    }
  }
  filestream.close();
  return value; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value{};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "Uid:") {return value;}
    }
  }
  filestream.close();
  return value; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string user{};
  string x;
  string s_pid;
  std::ifstream filestream(kPasswordPath);
  if(filestream.is_open()){
    while(std::getline(filestream,line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> s_pid;
      if (s_pid == Uid(pid)) { return user; }
    }
  }
  filestream.close();
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string s_value;
  long value{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i<=22; ++i){ linestream >> s_value;}
    value = std::stol(s_value)/sysconf(_SC_CLK_TCK);
  }
  filestream.close();
  return value; 
}