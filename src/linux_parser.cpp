#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <filesystem>

#include "linux_parser.h"

using std::stof;
using std::stol;
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
  filestream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, second_term, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> second_term >> kernel;
  }
  stream.close();
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  string line; 
  string key;

  float totalMem{1};
  float freeMem{1};

  // Creation of the stream for file
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if(stream.is_open())
  {
    // Cicle on the lines of file
    while(std::getline(stream, line))
    {
      std::istringstream line_stream(line);
      
      // Set the string in the key var
      line_stream >> key;

      // Check if key contains "MemAvalaible" or "MemTotal"
      // and assign values
      if(key == filterMemTotalString)
      {
        line_stream >> totalMem;
      }
      else if(key == filterMemAvalaibleString)
      {
        line_stream >> freeMem;
        // Stop the while
        break;
      }
    }
  }
  stream.close();
  // To calculate free memory i will this formula:(Total Mem - Free Mem) / Total Mem
    return (totalMem - freeMem) / totalMem;
}

// Read and return the system uptime
long LinuxParser::UpTime() 
{
  long up_time{0};
  string line;
  string up_time_string;

  // Open file in a stream
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream up_time_stream(line);
    up_time_stream >> up_time_string;
  }

  stream.close();

  // Made a conversion from string to interger value in a string
  // https://en.cppreference.com/w/cpp/string/basic_string/stol
  up_time = std::stol(up_time_string);

  return up_time;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) 
{
  long total_time;
  string line;
  string content;
  vector<string> contents;

  // Put file content in a stream
  // By to_string function i will convert a numeric value to a 
  // string to concatenate the string correctly
  // https://en.cppreference.com/w/cpp/string/basic_string/to_string
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open())
  {
    // Get the line from a stream
    std::getline(stream, line);

    // Put the line in a string stream
    std::istringstream string_line_stream(line);

    // Put the string stream in a string and add to collection
    while(string_line_stream >> content)
    {
      contents.push_back(content);
    }
  }
  stream.close();

  // Get from the collection of contents the values of content of my interesting
  long u_time{0};
  if(LinuxParser::IsNumber(contents[13]))
  {
    // Convert string to long by standard function stol
    // http://www.cplusplus.com/reference/string/stol/
    u_time = std::stol(contents[13]);
  }

  long s_time{0};
  if(LinuxParser::IsNumber(contents[14]))
    s_time = std::stol(contents[14]);

  long cu_time{0};
  if(LinuxParser::IsNumber(contents[15]))
    cu_time = std::stol(contents[15]);

  long cs_time{0};
  if(LinuxParser::IsNumber(contents[16]))
    cs_time = std::stol(contents[16]);

  total_time = cs_time + cu_time + s_time + u_time;

  // Conversion in tick
  return total_time / sysconf(_SC_CLK_TCK);

}

// Helper function to check if a string is a number
bool LinuxParser::IsNumber(std::string s)
{
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it))
  {
    ++it;
  }

  return (!s.empty() && it == s.end());
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{
  // Get Cpu Usage
  vector<string> jiffies = CpuUtilization();

  // From this collection I get the values of actived jiffies and sum to obtain the number of active
  // jiffies

  long total_actived_jiffies = stol(jiffies[CPUStates::kIRQ_]) +  stol(jiffies[CPUStates::kNice_]) +
                               stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]) +
                               stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kUser_]);
  return total_actived_jiffies;

}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{
  vector<string> jiffies = CpuUtilization();
  long total_idle_jiffies = stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
  return total_idle_jiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{
  string line;
  string cpu;
  string content;
  vector<string> jiffies;

  // Open file in a stream
  std::ifstream stream(kProcDirectory + kStatFilename);

  if(stream.is_open())
  {
    // Get a line of file
    std::getline(stream, line);

    // Put line a stream string
    std::istringstream line_stream(line);

    line_stream >> cpu;

    while(line_stream >> content)
    {
      jiffies.push_back(content);
    }
  }

  stream.close();

  return jiffies;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{
  return ProcessCounterHelper(filterProcesses);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{
  return ProcessCounterHelper(filterRunningProcesses);
}

// With this function helper I get some types of process live in the system
// Using a string like a discriminator.
// With this implementation it's possibile to respect the DRY paradigm
int LinuxParser::ProcessCounterHelper(std::string check_string)
{
  if(check_string.length() > 0)
  {
    int num_processes{0};
    string spy_string;
    string line;

    // Open file in a stream
    std::ifstream stream(kProcDirectory + kStatFilename);

    if(stream.is_open())
    {
      while (std::getline(stream, line))
      {
        std::istringstream line_stream(line);
        line_stream >> spy_string;

        if(spy_string == check_string)
        {
          line_stream >> num_processes;
          break;
        }

      }
    }
    stream.close();

    return num_processes; 
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{
  string command;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) 
  {
    std::getline(stream, command);
  }
  stream.close();

  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
  string line;
  string check_string;
  string memory_string;
  long memory;
  // I replace VmSize with VmRSS because VmRSS gives the exact physical memory being used as a part of Physical RAM

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      linestream >> check_string;
      if (check_string == filterProcMem) {
        linestream >> memory;
        memory = memory / 1024;
        memory_string = std::to_string(memory);
        break;
      }
    }
  }
  stream.close();

  return memory_string;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{ 
  string line;
  string check_string;
  string uid;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      linestream >> check_string;
      if (check_string == filterUID) 
      {
        linestream >> uid;
        break;
      }
    }
  }
  stream.close();

  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid)
{
  string uid = Uid(pid);
  string id;
  string second_element;
  string tmp;
  string line;
  string name = "EMPTY";

  std::ifstream stream(kPasswordPath);

  if (stream.is_open()) 
  {
    while (std::getline(stream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      linestream >> tmp >> second_element >> id;

      // If the pid is equal to current id then I have founded
      // the name of the user that have started the process
      if (id == uid) 
      {
        name = tmp;
        break;
      }
    }
  }
  stream.close();

  return name;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{
  string line;
  string content;
  vector<string> contents;
  long start_time{0};

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) 
  {
    std::getline(stream, line);
    std::istringstream linestream(line);

    while (linestream >> content) 
    {
      contents.push_back(content);
    }
  }

  stream.close();

  // Try to get the start_time of process but if it's not a long value
  // I set the value to 0 and return with catching the exception
  try 
  {
    start_time = stol(contents[21]) / sysconf(_SC_CLK_TCK);

  } catch (...) 
  {
    // The defualt valure of start_time is already 0, it's redundant, but for
    // code readbility reason I set another time the value.
    start_time = 0;
  }
  // to get unit of time it has been running since start I subtract it from the UpTime()
  return UpTime() - start_time;
}