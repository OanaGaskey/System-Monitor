#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int h{0};
  int m{0};
  int s{0};
 
  h = seconds/3600;
  m = seconds%3600;
  s = m%60;
  m = m/60;
  return std::to_string(h)+":"+std::to_string(m)+":"+std::to_string(s);
}