#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <locale>
#include <cmath>

#include "linux_parser.h"
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

using std::stoi;
using std::stol;
using std::cout;

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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

//grab_components Function 
vector<string> grab_components(const string& line){
  std::istringstream iss(line);
  std::istream_iterator<string> beg(iss), end;
  vector<string> values(beg, end);
  return values;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  
  const string totalMemory = "Memory Total";
  const string freeMemory = "Memory Free";
  const string availableMemory = "Memory Available";
  string terms[] = {totalMemory, freeMemory, availableMemory};

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line;
  string term2;
  float mem;
  std::map<string, float> mp;

  if (filestream.is_open()) {
    for (string term : terms) {
      while (getline(filestream, line)){
        vector<string> values = grab_components(line);
        if(term.compare(0, values[0].size(), values[0])){
          mem = stof(values[1]);
          mp[term] = mem;
          break;
        }
      }
    }
  }

  return (mp[totalMemory] - mp[freeMemory]) / mp[totalMemory];   
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  string uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  
  return stol(uptime); 
  
  }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  
  vector<string> all_jiffies = LinuxParser::CpuUtilization();
  float sum = 0;
  for(string j : all_jiffies){
    sum += std::stof(j);
  }
  return sum; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
    float utime; 
    float stime; 
    float cutime; 
    float cstime; 
    float starttime;
    const string directory = kProcDirectory + to_string(pid) + kStatFilename;

    std::ifstream filestream(directory);
    string line;
    getline(filestream, line);
    vector<string> values = grab_components(line);

    utime = std::stof(values[13]);
    stime = std::stof(values[14]);
    cutime = std::stof(values[15]);
    cstime = std::stof(values[16]);
    starttime = std::stol(values[21]);

    long int uptime = LinuxParser::UpTime();
    float herts = sysconf(_SC_CLK_TCK);

    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime / herts);

  return ((total_time / herts) / seconds) * 100;   
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> all_jiffies = LinuxParser::CpuUtilization();
  float sum = 0;
  all_jiffies.erase(all_jiffies.begin() + 3);
  for (string j : all_jiffies){
    sum+= std::stof(j);
  }
  return sum;   
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> all_jiffies = LinuxParser::CpuUtilization();
  return std::stof(all_jiffies[3]); 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  const string directory = kProcDirectory + kStatFilename;
  std::ifstream filestream(directory);
  string line;
  vector<string> numbers;
  getline(filestream, line);
  vector<string> values = grab_components(line);
  for (std::size_t i = 1; i < values.size(); i++){
    numbers.push_back(values[i]);
  }  
  
  return numbers; 
}

//extract_match
string LinuxParser::extract_match(const string& directory, const string& to_match){
  std::ifstream stream(directory);
  string val;
  string line;

  if (stream.is_open()){
    while(getline(stream, line)) {
      vector<string> values = grab_components(line);
      if (!to_match.compare(0, to_match.size(), values[0])){
        val = values[1];
        break;
      }
    }
  }
  return val;

}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  const string directory = kProcDirectory +kStatFilename;
  const string proc = "processes";
  string val = LinuxParser::extract_match(directory, proc);
  return stoi(val);   
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  //std::cout << "Entering Inside the RunningProcess " << "\n";
  const string directory = kProcDirectory + kStatFilename;
  const string rpoc = "procs_running";
  string val = LinuxParser::extract_match(directory, rpoc);  
 // std::cout << "Exiting Inside the RunningProcess " << "\n";
  return stoi(val); 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string cmd;
  const string directory = kProcDirectory + std::to_string(pid) + kCmdlineFilename;
  std::ifstream filestream(directory);
  getline(filestream, cmd);
  vector<string> values = grab_components(cmd);
  for (string v : values){
    cmd.append(v);
    return cmd;
  }

  return cmd;   
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value, units;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value >> units){
        if(key == "VmSize:"){
          return to_string(stoi(value)/ 1024);
        }
      }

    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string uid;
  const string directory = kProcDirectory + std::to_string(pid) + kStatusFilename;
  const string match = "Uid:";
  uid = LinuxParser::extract_match(directory, match);
  return uid;   
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  const string directory = kPasswordPath;
  string uid = LinuxParser::Uid(pid);
  std::ifstream filestream(directory);
  string line;
  string user;
  std::size_t found;
  while (getline(filestream, line)){
    found = line.find(uid);
    if(found != std::string::npos){
      found = line.find(":");
      return line.substr(0, found);
    }
  }
  return string(); 
 }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  const string directory = kProcDirectory + to_string(pid) + kStatFilename;
  std::ifstream filestream(directory);
  string line;
  getline(filestream, line);
  vector<string> values = grab_components(line);
  long ticks = std::stol(values[21]);
  return ticks / sysconf(_SC_CLK_TCK); 
}