#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a vector of Process composed of the system's processes
vector<Process>& System::Processes() {
  //get a vector of all PIDs from LinuxParser and add them to the vector of Processes
  vector<int> pidList = LinuxParser::Pids();
  
  for (int& pid : pidList) {
    processes_.emplace_back(Process(pid));
  }
  //sort processes by CPU Utilization
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

// return Kernel Version from LinuxParser 
std::string System::Kernel() { 
  return LinuxParser::Kernel(); }

// return total Memory Utilization from LinuxParser 
float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization(); }

// return OS Name from LinuxParser 
std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system from Linux Parser
int System::RunningProcesses() { 
  return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system from LinuxParser
int System::TotalProcesses() { 
  return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running from LinuxParser
long int System::UpTime() { 
  return LinuxParser::UpTime(); }
  