#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

std::map<std::string, std::string> mapMemStat = {};

// An example of how to read data from the filesystem
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
  string os, version, kernel;
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

// TODO: Read and return the system memory utilization
// Memory Utilization = 1.0 - (free_memory / (total_memory - buffers))

float LinuxParser::MemoryUtilization() {
  float memTotal{0};
  float memFree{0};
  float buffers{0};

  createMemStats();
  memTotal = stoi(mapMemStat["MemTotal"]);
  memFree = stoi(mapMemStat["MemFree"]);
  buffers = stoi(mapMemStat["Buffers"]);

  float memUtil = 1.0 - (memFree / (memTotal - buffers));

  return memUtil;
}

// get uptime from /proc/uptime and return first of the two
// vales as long int.
long LinuxParser::UpTime() {
  string line;
  string uptimeStr;
  long uptime;

  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimeStr;
  }
  uptime = stol(uptimeStr);

  stream.close();
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
// long LinuxParser::Jiffies() { return 0; }

float LinuxParser::CpuUtilizationProc(int pid) {
  std::vector<long> procCpuUtil;
  std::string value;
  long totaltime;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    for (int i = 0; i < 17; ++i) {
      filestream >> value;
      // 13 = utime; 14 = stime; 15 = cutime; 16 = cstime; 21 = starttime;

      switch (i) {
        case 13:
          procCpuUtil.emplace_back(stol(value));
          break;
        case 14:
          procCpuUtil.emplace_back(stol(value));
          break;
        case 15:
          procCpuUtil.emplace_back(stol(value));
          break;
        case 16:
          procCpuUtil.emplace_back(stol(value));
          break;
        default:
          break;
      }
    }
  }

  totaltime = procCpuUtil[0] + procCpuUtil[1] + procCpuUtil[2] + procCpuUtil[3];
  float cpu_usage =
      ((totaltime * 1. / (float)sysconf(_SC_CLK_TCK)) / (float)UpTime(pid));

  filestream.close();
  return cpu_usage;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> cpuStats;
  long jiffies;
  cpuStats = CpuUtilization();

  jiffies =
      stol(cpuStats[CPUStates::kUser_]) + stol(cpuStats[CPUStates::kNice_]) +
      stol(cpuStats[CPUStates::kSystem_]) + stol(cpuStats[CPUStates::kIRQ_]) +
      stol(cpuStats[CPUStates::kSoftIRQ_]) +
      stol(cpuStats[CPUStates::kSteal_]) + stol(cpuStats[CPUStates::kGuest_]) +
      stol(cpuStats[CPUStates::kGuestNice_]);

  return jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idleJiffies;
  std::vector<std::string> cpuStats;

  cpuStats = CpuUtilization();
  idleJiffies =
      stol(cpuStats[CPUStates::kIdle_]) + stol(cpuStats[CPUStates::kIOwait_]);

  return idleJiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::vector<string> cpuData;
  std::string token;
  std::string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> token) {
        if (token == "cpu") {
          while (linestream >> token) cpuData.push_back(token);
          return cpuData;
        }
      }
    }
  }

  return cpuData;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcs{0};
  std::string firstColumn;
  std::string secColumn;
  std::string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> firstColumn) {
        if (firstColumn == "processes") {
          linestream >> secColumn;
        }
      }
    }
  }
  totalProcs = stoi(secColumn);

  return totalProcs;
}

// read number of running prcesses from /proc/stat
// read as filestream and linestrean and return integer
// of running processes
int LinuxParser::RunningProcesses() {
  int runningProcs{0};
  std::string firstColumn;
  std::string secColumn;
  std::string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> firstColumn) {
        if (firstColumn == "procs_running") {
          linestream >> secColumn;
        }
      }
    }
  }
  runningProcs = stoi(secColumn);

  return runningProcs;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
  }

  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string stringRam;
  std::string token;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (filestream.is_open()) {
    while (filestream >> token) {
      if (token == "VmData:") {
        if (filestream >> stringRam) {
          stringRam = to_string(stoi(stringRam) / 1024);
          filestream.close();
          return stringRam;
        }
      }
    }
  }
  filestream.close();
  return "0";
}

// Read and return the user ID associated with a proces
string LinuxParser::Uid(int pid) {
  std::string key, value;
  std::string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          filestream.close();
          return value;
        }
      }
    }
  }
  filestream.close();
  return " ";
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string line, name, c, uid;

  std::ifstream filestream(LinuxParser::kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> c >> uid;
      if (uid == Uid(pid)) return name;
    }
  }
  filestream.close();
  return string(" ");
}

//  Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long uptime = 0;
  std::string token;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    for (int i = 0; filestream >> token; ++i)
      if (i == 21) {
        uptime = UpTime() - (stol(token) / sysconf(_SC_CLK_TCK));
        filestream.close();
        return uptime;
      }
  }
  filestream.close();
  return uptime;
}

// creating and filling a Map with all memory statistics
void LinuxParser::createMemStats() {
  std::string line, key, value;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  while (std::getline(filestream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    linestream >> key >> value;
    mapMemStat[key] = value;
  }
  filestream.close();
}