#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

// (DONE) TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // string line, str;
  float memtotal = 0;
  float memfree = 0;
  float buffers = 0;
  float cached = 0;
  float shmem = 0; 
  float sreclaimable = 0;  // num;
  // https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
  // MemUsage = (MemTotal - MemFree) - (Buffers + Cached + SReclaimable - Shmem)
  // MemUsagePerc = MemUsage / MemTotal

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    while (getline(stream, line)) {
      string str;
      float num;
      std::istringstream linestream(line);
      linestream >> str >> num;

      if (str == filterMemTotalString)
        memtotal = num;
      else if (str == filterMemFreeString)
        memfree = num;
      else if (str == filterMemBuffers)
        buffers = num;
      else if (str == filterMemCached)
        cached = num;
      else if (str == filterMemShmem)
        shmem = num;
      else if (str == filterMemSReclaimable) {
        sreclaimable = num;
        break;  // We know that this is the last number to extract
      }
    }
  }

  stream.close();

  // This gets pretty close to the percentage obtained from htop
  return (memtotal - memfree - (buffers + cached + sreclaimable - shmem)) /
         (memtotal + 1e-6);  // added a small number to avoid zero-division
}

// (DONE) TODO: Read and return the system uptime
long LinuxParser::UpTime() {

  long uptimeSec = getValueOfFile<long>(kProcDirectory + kUptimeFilename);

  return uptimeSec;
}

void LinuxParser::GetJiffies(long& user, long& nice, long& system, long& idle, long& iowait, long& irq, long& softirq, long& steal)
{
  string cpu = "";

  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);

    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal;
  }

  stream.close();

}

// Reference used for calculating jiffies
// https://knowledge.udacity.com/questions/129844
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
// https://man7.org/linux/man-pages/man5/proc.5.html

// (DONE) TODO: Read and return the number of jiffies for the system
// Currently not being used
long LinuxParser::Jiffies() {
  return LinuxParser::IdleJiffies() + LinuxParser::ActiveJiffies();
}

// (DONE) TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long activejiffies = 0;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);

    // burn through the first 13 columns
    // as we are interested numbers in the 14-17th columns (utime, stime,
    // cutime, cstime) Is there a better way?
    string temp;
    long num;
    for (int i = 0; i < 13; i++) {
      linestream >> temp;
    }

    for (int i = 0; i < 4; i++) {
      linestream >> num;
      activejiffies += num;
    }
  }

  stream.close();

  return activejiffies;
}

// (DONE) TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // According to
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // guess and guess_nice have already been included in user, and therefore we
  // don't need them
  // Could have mapped the string to the CPUStates, but I like my solution
  // better
  long user = 0;
  long nice = 0;
  long system = 0;
  long idle = 0;
  long iowait = 0;
  long irq = 0;
  long softirq = 0;
  long steal = 0;

  GetJiffies(user, nice, system, idle, iowait, irq, softirq, steal);

  // excluding the idle jiffies
  return user + nice + system + irq + softirq + steal; 
}

// (DONE) TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // pretty much identical to LinuxParser::ActiveJiffies, except for
  // the components being added. Would be better to merge the two into one
  // function using passed references (as arguments) to obtain idle and active
  // jiffies
  string cpu = "";
  long user = 0;
  long nice = 0;
  long system = 0;
  long idle = 0;
  long iowait = 0;
  long irq = 0;
  long softirq = 0;
  long steal = 0;

  GetJiffies(user, nice, system, idle, iowait, irq, softirq, steal);

  // excluding the non-idle jiffies
  return (idle + iowait);
}

// https://knowledge.udacity.com/questions/141752
// I still can't understand the intuition behind this approach
// CpuUtilization for processes and the CPU is performed in processor.cpp and process.cpp
// TODO: Read and return CPU utilization
// vector<string>  LinuxParser::CpuUtilization() {
// }

// (DONE) TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {

  int totalprocesses = findValueByKey<int>(filterProcesses, kProcDirectory + kStatFilename);

  return totalprocesses;
}

// (DONE) TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {

  int procsrunning = findValueByKey<int>(filterRunningProcesses, kProcDirectory + kStatFilename);

  return procsrunning;
}

// (DONE) TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    getline(stream, cmd);
  }

  // string cmd = getValueOfFile<string>(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

  return cmd;
}

// (DONE) TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {

  string ram = findValueByKey<string>(filterProcMem, kProcDirectory + std::to_string(pid) + kStatusFilename);

  // Slice the last three characters (trade accuracy for speed)
  // https://knowledge.udacity.com/questions/237138
  return ram.substr(0, ram.length() - 3);
}

// (DONE) TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {

  string uid = findValueByKey<string>(filterUID, kProcDirectory + std::to_string(pid) + kStatusFilename);

  return uid;
}

// (DONE) TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  // Tokenize the string using a custom separator
  // Modified from:
  // https://www.geeksforgeeks.org/how-to-split-a-string-in-cc-python-and-java/
  // to extract two substrings (uid and user)

  string uid = LinuxParser::Uid(pid);

  string user = string();
  const string sep = ":";
  bool found = false;

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string line;
    while (getline(stream, line)) {
      int start = 0;
      int end = line.find(sep);

      for (int pos = 0; pos < 3; pos++) {
        if (pos == 0) 
          user = line.substr(start, end - start);
        // if there is a match!
        else if (pos == 2 && uid == line.substr(start, end - start))
          found = true;
        
        start = end + sep.size();
        end = line.find(sep, start);
      }
      if (found) break;
    }
  }

  if (!found)
    return "NOTFOUND";
  else
    return user;
}

// (DONE) TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long starttime = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    std::istringstream linestream(line);

    // burn through the first 21 columns
    string temp;
    for (int i = 0; i < 21; i++) linestream >> temp;
    linestream >> starttime;
  }

  // Total elapsed time since the process started
  long seconds =  LinuxParser::UpTime()  - starttime / sysconf(_SC_CLK_TCK); 

  return seconds;
}
