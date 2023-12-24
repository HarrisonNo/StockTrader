#include <time.h>
#include <optional>
#include <inttypes.h>
#include <string>


#ifndef TIME_KEEPER_H
#define TIME_KEEPER_H









class time_keeper {
    private:
    //Variables
    time_t secs_since_1970;
    struct tm * timeinfo;



    public:








    //Functions
    time_keeper();

    void set_year(int16_t year);
    void set_month(int16_t month);
    void set_day(int16_t day);
    void set_hour(int16_t hour);
    void set_minute(int16_t minute);
    void set_second(int16_t second);

    void adjust_year(int16_t year);
    void adjust_month(int16_t month);
    void adjust_day(int16_t day);
    void adjust_hour(int16_t hour);
    void adjust_minute(int16_t minute);
    void adjust_second(int16_t second);




};

//Leave funcs outside class?


























#endif