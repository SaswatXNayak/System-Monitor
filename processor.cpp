#include "processor.h"

#include <iostream>

#include "linux_parser.h"

// (DONE) TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float nonidle = (float)LinuxParser::ActiveJiffies();
  float idle = (float)LinuxParser::IdleJiffies();

  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // https://rosettacode.org/wiki/Linux_CPU_utilization
  float totald = (nonidle + idle) - prev_total_;  //total_time_delta 
  float idled = idle - prev_idle_;  // idle_time_delta

  // Save for future use
  prev_total_ = nonidle + idle;
  prev_idle_ = idle;

  return (totald - idled) / totald;
}