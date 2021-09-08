#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
    // Constructor
    Process(int pid);

    //  Function
    int Pid();
    std::string User();                      // TODO: See src/process.cpp
    std::string Command();                   // TODO: See src/process.cpp
    float CpuUtilization();                  // TODO: See src/process.cpp
    std::string Ram();                       // TODO: See src/process.cpp
    long int UpTime();                       // TODO: See src/process.cpp
    bool operator<(Process& a);  // TODO: See src/process.cpp

// TODO: Declare any necessary private members
private:
    int pid_;
    std::string command_;
    long ram_;
    long up_time_;
    std::string user_;

    float usage_;
};

#endif