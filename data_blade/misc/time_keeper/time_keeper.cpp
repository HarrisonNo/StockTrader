#include "time_keeper.h"
#include "api.h"

#define TM_YEAR(T) T.tm_year
#define TM_MONTH(T) T.tm_mon
#define TM_DAY(T) T.tm_mday
#define TM_HOUR(T) T.tm_hour
#define TM_MINUTE(T) T.tm_min
#define TM_SECOND(T) T.tm_sec



time_keeper::time_keeper() {
    secs_since_1970 = CURRENT_TIME();
    struct tm * temp_time = localtime(&secs_since_1970);
    timeinfo = {};
    copy_pointer_into_struct(&temp_time, &timeinfo);
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
    return mktime(&timeinfo);
}


//NON CLASS FUNCTIONS
void copy_pointer_into_struct(std::tm **temp_ptr, std::tm *real_val) {
    TM_YEAR((*real_val)) = TM_YEAR((**temp_ptr));
    TM_MONTH((*real_val)) = TM_MONTH((**temp_ptr));
    TM_DAY((*real_val)) = TM_DAY((**temp_ptr));
    TM_HOUR((*real_val)) = TM_HOUR((**temp_ptr));
    TM_MINUTE((*real_val)) = TM_MINUTE((**temp_ptr));
    TM_SECOND((*real_val)) = TM_SECOND((**temp_ptr));
}