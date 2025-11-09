#include "process.h"
#include "linux_parser.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include <math.h>
#include <set>

#include <iostream>

using std::string;
using std::to_string;
using std::vector;

std::map<int, std::tuple<float, float>> Process::process_loads;

// (DONE) TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// (DONE) TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { 
  return cpu_usage_;
}

// The output is similar to htop
// Not sure if the formula is correct, though.
// Some of the utilizations are slightly overestimated, and some are underestimated
void Process::ComputeCpuUtilization() {
  float total_time = (float)LinuxParser::ActiveJiffies(pid_);
  float elapsed = (float)LinuxParser::UpTime(pid_);

  float total_time_prev = 0, elapsed_prev = 0;
  if (Process::process_loads.find(pid_) != Process::process_loads.end()) {
    total_time_prev = std::get<0>(Process::process_loads[pid_]);
    elapsed_prev = std::get<1>(Process::process_loads[pid_]);
  }
  
  float totald, idled;
  // Need to make sure the timestamps don't coincide
  // So we can sample the CPU load at two distinct timestamps
  if (elapsed - elapsed_prev > 0) {
    float idle = elapsed - total_time;
    float idle_prev = elapsed_prev - total_time_prev;

    totald = elapsed - elapsed_prev;
    idled = idle - idle_prev;

    // Save the timing for future use
    Process::process_loads[pid_] = std::make_tuple(total_time, elapsed);

    cpu_usage_ = ((totald - idled) / sysconf(_SC_CLK_TCK)) / totald;
  }
  else {
    // Process just launched
    if (elapsed == 0)
      cpu_usage_ = 0;
    else
      cpu_usage_ = (total_time / sysconf(_SC_CLK_TCK)) / elapsed;
  }
}

// (DONE) TODO: Return the command that generated this process
string Process::Command() { 
  return LinuxParser::Command(pid_);
}

// (DONE) TODO: Return this process's memory utilization
string Process::Ram() { 
  return LinuxParser::Ram(pid_); 
}

// (DONE) TODO: Return the user (name) that generated this process
string Process::User() { 
  return LinuxParser::User(pid_); 
}

// (DONE) TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
  return LinuxParser::UpTime(pid_);
}

//(DONE)  TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  return a.CpuUtilization() < CpuUtilization(); 
}


// Added helper function to prevent the obsolete processes from piling up
void Process::RemoveOldProcesses(const vector<int>& new_pids) {
  // To ensure O(n) time complexity, we introduce another set, 
  // which results in O(n) space complexity
  std::set<int> pids; // O(1) lookup time

  for (int i : new_pids)
    pids.insert(i);

  std::map<int, std::tuple<float, float>>::iterator it = Process::process_loads.begin();

  while (it != Process::process_loads.end())
  {
    // Cannot find the process (has died)
    if (pids.find(it->first) == pids.end())
      it = Process::process_loads.erase(it);
    else 
      ++it;
  }

}