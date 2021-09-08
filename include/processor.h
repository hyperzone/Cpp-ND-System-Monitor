#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
    float CurrentTotal();
    float CurrentActived();
    float CurrentInIdle();
    float PrevTotal();
    float PrevActivated();
    float PrevInIdle();
    float Utilization(); 
    void UpdateProcessor(long total, long idle, long active);

  // Declare any necessary private members
 private:
    long total_;
    long active_;
    long idle_;

};

#endif