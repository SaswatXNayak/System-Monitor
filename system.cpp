#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

#include <iostream>

using std::set;
using std::size_t;
using std::string;
using std::vector;

// (DONE) TODO: Return the system's CPU
Processor& System::Cpu() { 
  return cpu_; 
}

// (DONE) TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> process_ids = LinuxParser::Pids();

  // Clear the old processes
  processes_.clear();
  
  for (int pid : process_ids) {
    // Not sure why LinuxParser::Pids() picks up phantom processes
    // Workaround: only insert the processes that have a valid command
    Process process(pid);
    if (!process.Command().empty())
      processes_.push_back(process);
  }

  std::sort(processes_.begin(), processes_.end(), []( Process const& lhs, Process const& rhs) {
    return lhs < rhs;
  });

  // Clean up the old process timings
  Process::RemoveOldProcesses(process_ids);

  return processes_;
}

// (DONE) TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// (DONE) TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// (DONE) TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// (DONE) TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// (DONE) TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// (DONE) TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }