#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  active_jiffies_ = LinuxParser::ActiveJiffies();
  total_jiffies_  = LinuxParser::Jiffies();
  return static_cast<float>(active_jiffies_)/static_cast<float>(total_jiffies_); 
}