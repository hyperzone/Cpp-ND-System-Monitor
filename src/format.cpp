#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds)
{
    const int SECOND_IN_HOUR = 3600;
    const int MINUTE_IN_HOUR = 3600;
    const int SECOND_IN_MINUTE = 60;

    int hours{0};
    int mins{0};
    int secs{0};

    // I have made a mistake with the calculation of up time, 
    // I have made the needs correction to the formula.
    hours = seconds / SECOND_IN_HOUR;
    mins = (seconds % SECOND_IN_HOUR)/MINUTE_IN_HOUR;
    secs = seconds % SECOND_IN_MINUTE;

    std::string hh;
    std::string mm;
    std::string ss;

    // Formatting my string in HH:mm::ss format (In the previous version I have forgot this detail)
    if(hours < 10)
        hh = "0" + std::to_string(hours);
    else
        hh = std::to_string(hours);
    if(mins < 10)
        mm = "0" + std::to_string(mins);
    else
        mm = std::to_string(mins);
    if(secs < 10)
        ss = "0" + std::to_string(secs);
    else
        ss = std::to_string(secs);

    return hh + ":" + mm + ":" + ss;
}