#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// Keywords
const std::string filterProcesses("processes");
const std::string filterRunningProcesses("procs_running");
const std::string filterMemTotalString("MemTotal:");
const std::string filterMemFreeString("MemFree:");
const std::string filterMemBuffers("Buffers:");
const std::string filterMemCached("Cached:");
const std::string filterMemShmem("Shmem:");
const std::string filterMemSReclaimable("SReclaimable:");
const std::string filterCpu("cpu");
const std::string filterUID("Uid:");
const std::string filterProcMem("VmData:");

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

// templatized line search by key
template <class T>
T findValueByKey(const std::string& keyFilter, const std::string& filename)
{
  std::string key;
  std::string line;
  T value;

  std::ifstream stream(filename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;

      if (key == keyFilter) {
        linestream >> value;
        break;
      }
    }
  }
  stream.close();

  return value;
}

template <class T>
T getValueOfFile(const std::string& filename)
{
  T value;
  std::string line;

  std::ifstream stream(filename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }

  stream.close();

  return value;
}

// https://knowledge.udacity.com/questions/141752
// I still can't understand the intuition behind this approach
// std::vector<std::string> CpuUtilization();

void GetJiffies(long& user, long& nice, long& system, long& idle, long& iowait, long& irq, long& softirq, long& steal);
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid); 
long int UpTime(int pid);
};  // namespace LinuxParser

#endif