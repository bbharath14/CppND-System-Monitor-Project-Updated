#include <string>
#include<time.h>
#include<math.h>
#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    time_t seconds1(seconds); // you have to convert your input_seconds into time_t
    tm *p = gmtime(&seconds1);
    std::string t ="";
    t+=std::to_string(p->tm_hour)+":";
    t+=std::to_string(p->tm_min)+":";
    t+=std::to_string((int)roundf(p->tm_sec*100)/100);
    return t;
}