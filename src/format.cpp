#include "format.h"


#define HOUR 3600
#define MIN 60

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS

std::string Format::ElapsedTime(long seconds) {
  int sec, min, hours;
  std::string time;

  hours = seconds / HOUR;
  seconds %= 3600;
  min = (int)(seconds / MIN);
  sec = (int)(seconds % MIN);

  std::ostringstream result;
  result << std::setfill('0') << std::setw(2) << hours << ":"
         << std::setfill('0') << std::setw(2) << min << ":"
         << std::setfill('0') << std::setw(2) << sec;

  return result.str();
}