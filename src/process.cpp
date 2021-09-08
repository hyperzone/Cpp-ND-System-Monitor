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

// Constructor
Process::Process(int pid) {
  pid_ = pid;
  command_ = LinuxParser::Command(pid);
  std::string ram = LinuxParser::Ram(pid);
  ram_ = std::stol(ram);
  up_time_ = LinuxParser::UpTime(pid);
  user_ = LinuxParser::User(pid);

  long seconds = LinuxParser::UpTime() - up_time_;
  long totaltime = LinuxParser::ActiveJiffies(pid);
  try 
  {
    usage_ = float(totaltime) / float(seconds);

  } catch (...) {
    usage_ = 0;
  }
}

// Return this process's ID
int Process::Pid()
{
    return pid_;
}

// Return this process's CPU utilization
float Process::CpuUtilization() 
{
    return usage_;
}

// Return the command that generated this process
string Process::Command() 
{
    // If command string is length more than 40 chars, I truncate it and add a some points at the end
    if(command_.length() > 40)
    {
        return command_.substr(0, 39) + "...";
    }
    else
        return command_;
}

// Return this process's memory utilization
string Process::Ram() 
{
    return std::to_string(ram_);
}

// Return the user (name) that generated this process
string Process::User() 
{
    return user_;
}

// Return the age of this process (in seconds)
long int Process::UpTime() 
{
    return up_time_;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process& a)
{
    // if(CpuUtilization() < a.CpuUtilization())
    // {
    //     return true;
    // }
    // return false;
    
    // Semplification
  return CpuUtilization() < a.CpuUtilization();
}