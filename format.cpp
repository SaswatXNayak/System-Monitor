#include "format.h"

#include <cmath>
#include <iostream>
#include <string>

using std::string;

string Format::ToStrWithLeadingZero(long num) {
  if (num < 10)
    return '0' + std::to_string(num);
  else
    return std::to_string(num);
}

// (DONE) TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
// Bonus: if HH exceeds 99, a day counter is prepended (e.g., if elapsed time is
// 108:00:00, then the visualization becomes '4 days, 12:00:00'. ), following
// the htop format 
string Format::ElapsedTime(long seconds) {
  long hh, mm, ss;
  // long dd;
  std::string dd_str = "";

  hh = floor(seconds / SECS_IN_HR);
  seconds -= hh * SECS_IN_HR;

  mm = floor(seconds / SECS_IN_MIN);
  seconds -= mm * SECS_IN_MIN;

  ss = seconds;

  // (not implemented because the processes use the same format)
  // if (hh > 99) {
  //   dd = floor(hh / 24);
  //   hh -= dd * 24;
  //   if (dd > 1)
  //     dd_str = std::to_string(dd) + " days, ";
  //   else
  //     dd_str = std::to_string(dd) + " day, ";
  // }

  return dd_str + Format::ToStrWithLeadingZero(hh) + ":" +
         Format::ToStrWithLeadingZero(mm) + ":" +
         Format::ToStrWithLeadingZero(ss);
}
