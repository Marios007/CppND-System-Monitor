#include "processor.h"
#include "linux_parser.h"


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  
  long idle = LinuxParser::IdleJiffies();
  long active = LinuxParser::ActiveJiffies();

  long duration_active{active - prev_active};
  long duration_idle{idle - prev_idle};
  long duration{duration_active + duration_idle};
  
  float result = (float)(duration_active) / duration;
  prev_active = active;
  prev_idle = idle;

  return result;
}