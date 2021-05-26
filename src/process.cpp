#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization from LinuxParser
float Process::CpuUtilization() { 
    cpuUtil_=LinuxParser::CpuUtilizationProc(Pid());
     
   //  if (Pid() == 6710){
   //  std::ofstream textfile;
   //  textfile.open("test.txt");
   //  textfile << Pid() << " " << cpuUtil_ << "\n";
     
    return cpuUtil_;
   }

// Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(Pid()); }

// Return this process's memory utilization
string Process::Ram() { 
    ram_ = LinuxParser::Ram(Pid());
    return ram_; }

// Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return  cpuUtil_>a.cpuUtil_; }

bool Process::operator>(Process const& a) const {
    return cpuUtil_<a.cpuUtil_;
 }