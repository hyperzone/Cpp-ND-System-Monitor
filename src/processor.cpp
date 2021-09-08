#include "processor.h"
#include "linux_parser.h"


float Processor::CurrentTotal()
{
    return LinuxParser::Jiffies();
}

float Processor::CurrentActived()
{
    return LinuxParser::ActiveJiffies();
}

float Processor::CurrentInIdle()
{
    return LinuxParser::IdleJiffies();
}

float Processor::PrevTotal()
{
    return total_;
}

float Processor::PrevActivated()
{
    return active_;
}

float Processor::PrevInIdle()
{
    return idle_;
}

// Store the current state of the process in the system
void Processor::UpdateProcessor(long total, long idle, long active)
{
    total_ = total;
    idle_ = idle;
    active_ = active;
}

// Return the aggregate CPU utilization
float Processor::Utilization() 
{
    long total_prev{0};
    long total_current{0};
    long idle_prev{0};
    long idle_current{0};
    long active_current{0};

    total_current = CurrentTotal();
    idle_current = CurrentInIdle();
    active_current = CurrentActived();

    idle_prev = PrevInIdle();
    total_prev = PrevTotal();

    // Update the current state
    UpdateProcessor(total_current, idle_current, active_current);

    // Calculate the delta for measure the difference in system utilization
    // between two points in time relatively close to the present.
    double delta_idle = double(idle_current) - double(idle_prev);
    double delta_total = double(total_current) - double(total_prev);

    double usage = (delta_total - delta_idle) / delta_total;
    return usage;
}