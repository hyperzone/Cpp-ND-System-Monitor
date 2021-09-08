#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes()
{
    // Get all process PId
    vector<int> process_pids = LinuxParser::Pids();
    
    // Clear my collection
    processes_.clear();

    for(long unsigned int i = 0; i < process_pids.size(); i++)
    {
        // If memory used by process is not empty
        if(!LinuxParser::Ram(process_pids[i]).empty())
        {
            // It's possible to instantiate new process and add in a vector like here 
            //processes.push_back(process pids[i]); 
            // But I prefer code readability
            Process p(process_pids[i]);
            processes_.push_back(process_pids[i]);
        }
    }

    // Sort my vector in descending order
    std::sort(processes_.rbegin(), processes_.rend());
    
    return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() 
{
    return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() 
{
    return LinuxParser::MemoryUtilization();     
}

// Return the operating system name
std::string System::OperatingSystem() 
{
    return LinuxParser::OperatingSystem();
}

// Return the number of processes actively running on the system
int System::RunningProcesses() 
{
    return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses()
{
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() 
{
    return LinuxParser::UpTime();
}