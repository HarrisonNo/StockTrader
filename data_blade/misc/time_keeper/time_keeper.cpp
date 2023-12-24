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
void time_keeper::set_year(int16_t year) {
    TM_MONTH(timeinfo) = year;
}


void time_keeper::set_month(int16_t month) {
    TM_MONTH(timeinfo) = month;
}

void time_keeper::set_day(int16_t day) {
    TM_DAY(timeinfo) = day;
}

void time_keeper::set_hour(int16_t hour) {
    TM_HOUR(timeinfo) = hour;
}

void time_keeper::set_minute(int16_t minute) {
    TM_MINUTE(timeinfo) = minute;
}

void time_keeper::set_second(int16_t second) {
    TM_SECOND(timeinfo) = second;
}

//ADJUST VALS
void time_keeper::adjust_year(int16_t year) {
    TM_MONTH(timeinfo) += year;
}

void time_keeper::adjust_month(int16_t month) {
    TM_MONTH(timeinfo) += month;
}

void time_keeper::adjust_day(int16_t day) {
    TM_DAY(timeinfo) += day;
}

void time_keeper::adjust_hour(int16_t hour) {
    TM_HOUR(timeinfo) += hour;
}

void time_keeper::adjust_minute(int16_t minute) {
    TM_MINUTE(timeinfo) += minute;
}

void time_keeper::adjust_second(int16_t second) {
    TM_SECOND(timeinfo) += second;
}










void time_keeper::adjust_and_set(std::optional<int16_t> year_adjustment, std::optional<int16_t> month_adj, std::optional<int16_t> day_adjustment, std::optional<int16_t> hour_adjustment, std::optional<int16_t> minute_adjustment, std::optional<int16_t> second_adjustment,
                                 std::optional<int16_t> year_set,        std::optional<int16_t> month_set,        std::optional<int16_t> day_set,        std::optional<int16_t> hour_set,        std::optional<int16_t> minute_set,        std::optional<int16_t> second_set) {
    

    if (year_set.has_value()) {
        timeinfo->tm_year = year_set.value();
    }
    if (month_set.has_value()) {
        timeinfo->tm_mon = month_set.value();
    }
    if (day_set.has_value()) {
        timeinfo->tm_mday = day_set.value();
    }
    if (hour_set.has_value()) {
        timeinfo->tm_hour = hour_set.value();
    }
    if (minute_set.has_value()) {
        timeinfo->tm_min = minute_set.value();
    }
    if (second_set.has_value()) {
        timeinfo->tm_sec = second_set.value();
    }

    timeinfo->tm_year += year_adjustment.value_or(0);
    timeinfo->tm_mon += month_adjustment.value_or(0);
    timeinfo->tm_mday += day_adjustment.value_or(0);
    timeinfo->tm_hour += hour_adjustment.value_or(0);
    timeinfo->tm_min += minute_adjustment.value_or(0);
    timeinfo->tm_sec += second_adjustment.value_or(0);

    secs_since_1970 = mktime(timeinfo);//Adjustments are handled by mktime, thank fuck 
}





#define HANDLE_SINGULAR_VALID_DATE \
        temp_badness_score = abs(((temp_second - requested_second)) + \
                                 ((temp_minute - requested_minute) * 60) + \
                                 ((temp_hour - requested_hour) * 360) + \
                                 ((temp_day - requested_day) * 8640) + \
                                 ((month - requested_month) * 259200) + \
                                 ((year - requested_year) * 94608000)); \
        if (final_badness_score == INT_FAST64_MAX || abs(temp_badness_score) < abs(final_badness_score)) { \
            final_price = temp_price;\
            final_badness_score = temp_badness_score; \
        }

/*
Input: Takes in requested year, month, day, hour, minute, and second with a deviation variable for each. Any unspecified requested variable will be replaced by current time. Normal deviation is 0
Output: Returns the requested price at the closest time to requested time within the deviation, -1 if no valid price
Description: Should we ever save some of the price points incore?
Assumptions:
*/

//TODO THIS WONT WORK THE WAY I WANT IT TO, optional vars can only be used at the end of the func, calling stock_price_at_time(requested_month = 10) wont work as cannot specify which input we are replacing
//Alternatives:
//struct time_keeper + defines?
//pass in string that holds all requested vars?

//https://stackoverflow.com/questions/16860960/how-should-one-use-stdoptional
double logical_ticker::stock_price_at_time(std::optional <int16_t> requested_year/* = INT16_MIN*/, std::optional int16_t requested_month/* = INT16_MIN*/, std::optional int16_t requested_day/* = INT16_MIN*/, int16_t requested_hour/* = INT16_MIN*/, int16_t requested_minute/* = INT16_MIN*/, int16_t requested_second/* = INT16_MIN*/,
                                           std::optional int16_t year_deviation/* = 0*/, std::optional int16_t month_deviation/* = 0*/, int16_t day_deviation/* = 0*/, int16_t hour_deviation/* = 0*/, int16_t minute_deviation/* = 0*/, int16_t second_deviation/* = 0*/) {    

//double logical_ticker::stock_price_at_time(std::optional <int16_t> requested_year/* = INT16_MIN*/, std::optional int16_t requested_month/* = INT16_MIN*/, std::optional int16_t requested_day/* = INT16_MIN*/, int16_t requested_hour/* = INT16_MIN*/, int16_t requested_minute/* = INT16_MIN*/, int16_t requested_second/* = INT16_MIN*/,
//                                           std::optional int16_t year_deviation/* = 0*/, std::optional int16_t month_deviation/* = 0*/, int16_t day_deviation/* = 0*/, int16_t hour_deviation/* = 0*/, int16_t minute_deviation/* = 0*/, int16_t second_deviation/* = 0*/) {    
    int_fast16_t max_year, min_year;
    int_fast16_t max_month, min_month, loop_month_min, loop_month_max, loop_catch_min, loop_catch_max;
    int_fast16_t max_day, min_day, temp_day;
    int_fast16_t max_hour, min_hour, temp_hour;
    int_fast16_t max_minute, min_minute, temp_minute;
    int_fast16_t max_second, min_second, temp_second;
    int_fast64_t temp_badness_score, final_badness_score = INT_FAST64_MAX;//How far off the mark are we from a perfect request, the lower the better
    std::fstream historical_price_file;
    double temp_price, final_price = -1;
    std::string ticker_file_location;

    _catch_invalid_dates(&requested_year, &requested_month, &requested_day, &requested_hour, &requested_minute, &requested_second);

    max_year = requested_year + year_deviation;
    min_year = requested_second - year_deviation;

    max_month = requested_month + month_deviation;
    min_month = requested_month - month_deviation;

    max_day = requested_day + day_deviation;
    min_day = requested_day - day_deviation;

    max_hour = requested_hour + hour_deviation;
    min_hour = requested_hour - hour_deviation;

    max_minute = requested_minute + minute_deviation;
    min_minute = requested_minute - minute_deviation;

    max_second = requested_second + second_deviation;
    min_second = requested_second - second_deviation;

    _date_corrections(&min_year, &min_month, &min_day, &min_hour, &min_minute, &min_second,
                      &max_year, &max_month, &max_day, &max_hour, &max_minute, &max_second);

    for (int year = min_year; year <= max_year; year++) {
        check_and_create_dirs(HISTORICAL_TICKER_YEAR_DIR(_ticker, std::to_string(year)));

        if (year == min_year && year == max_year) {
            loop_month_min = loop_catch_min = min_month;
            loop_month_max = loop_catch_max = max_month;
        }
        else if (year == max_year) {
            loop_month_max = loop_catch_max = max_month;
            loop_month_min = 0;
            loop_catch_min = -1;//Unhittable
        }
        else if (year == min_year) {
            loop_month_min = loop_catch_min = min_month;
            loop_month_max = 11;
            loop_catch_max = -1;//Unhittable
        }
        else {
            loop_month_min = 0;
            loop_catch_min = -1;//Unhittable
            loop_month_max = 11;
            loop_catch_max = -1;//Unhittable
        }

        for (int month = loop_month_min; month <= loop_month_max; month++) {
            historical_price_file.open(HISTORICAL_TICKER_MONTH_FILE(_ticker, std::to_string(year), std::to_string(month)), std::ios::in);
            if (month == loop_catch_max && month == loop_catch_min) {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    if (temp_day < min_day) {
                        //Not valid date
                        continue;
                    }
                    if (temp_day > max_day) {
                        //Hit end
                        goto end;
                    }
                    //Valid date
                    HANDLE_SINGULAR_VALID_DATE;
                }
            }
            else if (month == loop_catch_max) {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    if (temp_day > max_day) {
                        //Hit end
                        goto end;
                    }
                    //Valid date
                    HANDLE_SINGULAR_VALID_DATE;
                }
            }
            else if (month == loop_catch_min) {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    if (temp_day < min_day) {
                        //Hit end
                        goto end;
                    }
                    //Valid date
                    HANDLE_SINGULAR_VALID_DATE;
                }
            } else {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    //Valid date
                    HANDLE_SINGULAR_VALID_DATE;
                }
            }
            historical_price_file.close();
        }
    }
    //On hitting maximum range we goto here
    end:
    historical_price_file.close();

    //Returns -1 if no satisfactory price can be found within the requested margin
    return final_price;
}


#define HANDLE_MULTIPLE_VALID_DATE \
        array_bank[array_bank_cnt++] = temp_price; \
        if (array_bank_cnt >= calculation_limit) { \
            goto end; \
        } 


/*
Input:
Output: fills array_bank with valid stock prices and returns the number of prices it found in time range
Description: TODO move these time based funcs to another file and separate from class?
Assumptions:
*/
uint64_t logical_ticker::stock_prices_between_times(uint64_t calculation_limit, double * array_bank, int16_t min_year/* = INT16_MIN*/, int16_t min_month/* = INT16_MIN*/, int16_t min_day/* = INT16_MIN*/, int16_t min_hour/* = INT16_MIN*/, int16_t min_minute/* = INT16_MIN*/, int16_t min_second/* = INT16_MIN*/,
                                                                                                     int16_t max_year/* = INT16_MIN*/, int16_t max_month/* = INT16_MIN*/, int16_t max_day/* = INT16_MIN*/, int16_t max_hour/* = INT16_MIN*/, int16_t max_minute/* = INT16_MIN*/, int16_t max_second/* = INT16_MIN*/) {
    int16_t loop_month_max, loop_month_min, loop_catch_min, loop_catch_max;
    int16_t temp_day, temp_hour, temp_minute, temp_second, temp_price;
    uint64_t array_bank_cnt = 0;
    std::fstream historical_price_file;
    std::string ticker_file_location;

    _catch_invalid_dates(&min_year, &min_month, &min_day, &min_hour, &min_minute, &min_second);
    _catch_invalid_dates(&max_year, &max_month, &max_day, &max_hour, &max_minute, &max_second);
    _date_corrections(&min_year, &min_month, &min_day, &min_hour, &min_minute, &min_second, &max_year, &max_month, &max_day, &max_hour, &max_minute, &max_second);

    if (calculation_limit == 0) {
        return 0;
    }

    for (int year = min_year; year <= max_year; year++) {
        check_and_create_dirs(HISTORICAL_TICKER_YEAR_DIR(_ticker, std::to_string(year)));
        
        if (year == min_year && year == max_year) {
            loop_month_min = loop_catch_min = min_month;
            loop_month_max = loop_catch_max = max_month;
        }
        else if (year == max_year) {
            loop_month_max = loop_catch_max = max_month;
            loop_month_min = 0;
            loop_catch_min = -1;//Unhittable
        }
        else if (year == min_year) {
            loop_month_min = loop_catch_min = min_month;
            loop_month_max = 11;
            loop_catch_max = -1;//Unhittable
        }
        else {
            loop_month_min = 0;
            loop_catch_min = -1;//Unhittable
            loop_month_max = 11;
            loop_catch_max = -1;//Unhittable
        }

        for (int month = loop_month_min; month <= loop_month_max; month++) {
            historical_price_file.open(HISTORICAL_TICKER_MONTH_FILE(_ticker, std::to_string(year), std::to_string(month)), std::ios::in);
            if (month == loop_catch_max && month == loop_catch_min) {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    if (temp_day < min_day) {
                        //Not valid date
                        continue;
                    }
                    if (temp_day > max_day) {
                        //Hit end
                        goto end;
                    }
                    //Valid date
                    HANDLE_MULTIPLE_VALID_DATE;
                }
            }
            else if (month == loop_catch_max) {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    if (temp_day > max_day) {
                        //Hit end
                        goto end;
                    }
                    //Valid date
                    HANDLE_MULTIPLE_VALID_DATE;
                }
            }
            else if (month == loop_catch_min) {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    if (temp_day < min_day) {
                        //Hit end
                        goto end;
                    }
                    //Valid date
                    HANDLE_MULTIPLE_VALID_DATE;
                }
            } else {
                while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
                    //Valid date
                    HANDLE_MULTIPLE_VALID_DATE;
                }
            }
            historical_price_file.close();
        }
    }
    //On hitting maximum range we goto here
    end:
    historical_price_file.close();
    return array_bank_cnt;
}