#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifndef TIME_KEEPER_H
#define TIME_KEEPER_H


//LOOSE CALCULATIONS, SHOULD NOT BE WIDELY USED
#define MIN_TO_SEC(min) min * 60
#define HOUR_TO_SEC(hour) MIN_TO_SEC(hour) * 60
#define DAY_TO_SEC(day) HOUR_TO_SEC(day) * 60


enum operation{
    adjust_year,
    adjust_month,
    adjust_day,
    adjust_hour,
    adjust_minute,
    adjust_second,
    set_year,
    set_month,
    set_day,
    set_hour,
    set_minute,
    set_second
};

//Non class functions
void copy_pointer_into_struct(std::tm **temp_ptr, std::tm *real_val);


class time_keeper {
    private:
    //Variables
    time_t secs_since_1970;
    std::tm timeinfo;

    public:
    //Functions
    time_keeper();

    void set_year(int year);
    void set_month(int month);
    void set_day(int day);
    void set_hour(int hour);
    void set_minute(int minute);
    void set_second(int second);

    void adjust_year(int year);
    void adjust_month(int month);
    void adjust_day(int day);
    void adjust_hour(int hour);
    void adjust_minute(int minute);
    void adjust_second(int second);

    time_t finalize();
};

#endif