#include <time.h>


#ifndef TIME_KEEPER_H
#define TIME_KEEPER_H


#define TIME1(A) time_keeper * tk; tk->A; tk->finalize()
#define TIME2(A, B) time_keeper * tk; tk->A; tk->B; tk->finalize()
#define TIME3(A, B, C) time_keeper * tk; tk->A; tk->B; tk->C; tk->finalize()
#define TIME4(A, B, C, D) time_keeper * tk; tk->A; tk->B; tk->C; tk->D; tk->finalize()
#define TIME5(A, B, C, D, E) time_keeper * tk; tk->A; tk->B; tk->C; tk->D; tk->E; tk->finalize()
#define TIME6(A, B, C, D, E, F) time_keeper * tk; tk->A; tk->B; tk->C; tk->D; tk->E; tk->F; tk->finalize()



class time_keeper {
    private:
    //Variables
    time_t secs_since_1970;
    struct tm * timeinfo;

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