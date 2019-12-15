#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  float Utilization(); 

 private:
    long prevIdle;
    long Idle;
    long prevNonIdle;
    long nonIdle;
    long prevTotal;
    long total;
};

#endif