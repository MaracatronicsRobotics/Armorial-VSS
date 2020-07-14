#include "timer.h"

/* Timer() function
 * Constructor
 *
*/
Timer::Timer()
{

}

/* start() function
 * Start var "time1" with actual time
 *
*/
void Timer::start() {
    clock_gettime(CLOCK_REALTIME, &time1);
}

/* start() function
 * Start var "time2" with actual time
 *
*/
void Timer::stop() {
    clock_gettime(CLOCK_REALTIME, &time2);
}

/* timesec() function
 * Returns time in seconds
 *
*/
double Timer::timesec() {
    return timensec()/1E9;
}

/* timemsec() function
 * Returns time in miliseconds
 *
*/
double Timer::timemsec()    {
    return timensec()/1E6;
}

/* timeusec() function
 * Returns time in microseconds
 *
*/
double Timer::timeusec()    {
    return timensec()/1E3;
}

/* timensec() function
 * Returns time in nanoseconds
 *
*/
double Timer::timensec()    {
    return (time2.tv_sec*1E9 + time2.tv_nsec) - (time1.tv_sec*1E9 + time1.tv_nsec);
}
