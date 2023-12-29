#include "time_keeper.h"


#define TM_YEAR(T) T->tm_year
#define TM_MONTH(T) T->tm_mon
#define TM_DAY(T) T->tm_mday
#define TM_HOUR(T) T->tm_hour
#define TM_MINUTE(T) T->tm_min
#define TM_SECOND(T) T->tm_sec



time_keeper::time_keeper() {
    secs_since_1970 = time(NULL);
    timeinfo = localtime(&secs_since_1970);
}

//SET VALS
void time_keeper::set_year(int year) {
    TM_MONTH(timeinfo) = year;
}

void time_keeper::set_month(int month) {
    TM_MONTH(timeinfo) = month;
}

void time_keeper::set_day(int day) {
    TM_DAY(timeinfo) = day;
}

void time_keeper::set_hour(int hour) {
    TM_HOUR(timeinfo) = hour;
}

void time_keeper::set_minute(int minute) {
    TM_MINUTE(timeinfo) = minute;
}

void time_keeper::set_second(int second) {
    TM_SECOND(timeinfo) = second;
}

//ADJUST VALS
void time_keeper::adjust_year(int year) {
    TM_MONTH(timeinfo) += year;
}

void time_keeper::adjust_month(int month) {
    TM_MONTH(timeinfo) += month;
}

void time_keeper::adjust_day(int day) {
    TM_DAY(timeinfo) += day;
}

void time_keeper::adjust_hour(int hour) {
    TM_HOUR(timeinfo) += hour;
}

void time_keeper::adjust_minute(int minute) {
    TM_MINUTE(timeinfo) += minute;
}

void time_keeper::adjust_second(int second) {
    TM_SECOND(timeinfo) += second;
}

//Finalize any changes
time_t time_keeper::finalize() {
    secs_since_1970 = mktime(timeinfo);
    return secs_since_1970;
}