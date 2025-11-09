#ifndef FORMAT_H
#define FORMAT_H

#include <string>

#define SECS_IN_MIN 60
#define SECS_IN_HR 3600
namespace Format {

// A helper function that converts a number to string
// and add a leading zero if needed (e.g., 1 -> '01')
std::string ToStrWithLeadingZero(long num);

std::string ElapsedTime(long times);  // TODO: See src/format.cpp
};                                    // namespace Format

#endif