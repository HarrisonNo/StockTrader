#include <thread>
#include <fstream>
#include "logical_ticker.h"
#include "assert_and_verify.h"
#include "directory_file_saving.h"


#define MAX_STOCK_PRICE_TIMEOUT 5 //Number of seconds we can use our stored stock price for a ticker before needing to check it again
#define MAX_KNOWN_SEC_TIMEOUT 21600 //Max number of seconds we can use our stored known amount before double checking (6 hours)
#define MIN_KNOWN_SEC_TIMEOUT 60 //Min number of seconds we have to double check before we can use our stored known amount

/*
Input:
Output:
Description:
Assumptions:
*/
logical_ticker::logical_ticker(std::string input_ticker, bool allow_boot_loading/* = true*/) {
    _time_last_checked_price = 0;
    _time_last_checked_amount = 0;
    _time_last_executed_transaction = 0;
    _known_stock_amount_owned = 0;
    _ticker = input_ticker;
    _can_sell_at_loss_default = 0;
    _transactions_list_stock_count = 0;
    if (allow_boot_loading) {
        _load_transactions();
    }
    amount_owned(true);
}


/*
Input:
Output:
Description: Save and clear new-ed memory, so far all we need to do is save, should this be optional? TODO
Assumptions:
*/
logical_ticker::~logical_ticker() {
    //Save ourselves
    save_self();
    //Then delete all new-ed memory
    while(!(_transactions.empty())) {
        list_insert * li = _transactions.front();
        _transactions.pop_front();
        delete(li);
    }
    
}


/*
Input:
Output:
Description: Save everything, so far just need to save transactions
Assumptions:
*/
void logical_ticker::save_self() {
    _save_transactions();
}


/*
Input:
Output: returns the amount owned, 0 on wrapper failure(!!!!!!)
Description: returns amount owned, minimizing wrapper calls if we already checked it recently 
Assumptions:
*/
uint32_t logical_ticker::amount_owned(bool force_check/* = false*/) {
    time_t current_time = time(NULL);
    uint_fast32_t discrepancy, returned_amount_owned;

    //If:
    //We are not forcing a wrapper call
    //We know the stock amount
    //Has not been more than MAX_KNOWN_SEC_TIMEOUT since the last time we checked the amount
    //Has at least been more than MIN_KNOWN_SEC_TIMEOUT since the last time we made a transaction
    //Then: return the internal value for amount_owned
    if (!force_check && _known_stock_amount_owned && ((current_time - _time_last_checked_amount) < MAX_KNOWN_SEC_TIMEOUT) && ((current_time - _time_last_executed_transaction) > MIN_KNOWN_SEC_TIMEOUT)) {
        return _amount_owned;
    }

    try {
        returned_amount_owned = AMOUNT_OWNED_CALL(_ticker);
    }
    catch (std::exception &e) {
        ASSERT(!"amount_owned wrapper failed");
         _known_stock_amount_owned = 0;
        return 0;
    }

    _amount_owned = returned_amount_owned;
    _time_last_checked_amount = current_time;
    discrepancy = _transactions_list_stock_count - _amount_owned;
    //Catch discrepancy with transactions
    if (discrepancy != 0) {
        _modify_transaction_list(_amount_owned - _transactions_list_stock_count);
        _known_stock_amount_owned = 0;//We had to modify the list, we may not know the real amount
    } else {
        _known_stock_amount_owned = 1;
    }

    return _amount_owned;
}


/*
Input:
Output: current stock price, 0 on wrapper failure(!!!!!!)
Description: gets the stock price, minimizing wrapper calls if we already checked it recently 
Assumptions:
*/
double logical_ticker::stock_price(bool force_check/* = false*/) {
    time_t current_time = time(NULL);

    if (!force_check && (current_time - _time_last_checked_price) > MAX_STOCK_PRICE_TIMEOUT) {
        return _stock_price;
    }

    try {
        _stock_price = STOCK_PRICE_CALL(_ticker);
    }
    catch (std::exception &e) {
        ASSERT(!"stock_price wrapper failed");
        //_trigger_wrapper_failure();//Increments var and checks if enough failures have occured within X seconds, if so then trigger a clean shutdown //TODO
        return 0;
    }

    _time_last_checked_price = current_time;
    _save_stock_price_at_time(current_time);

    return _stock_price;
}


/*
Input:
Output: amount we BELIEVE we purchased
Description:
Assumptions:
*/
uint32_t logical_ticker::purchase_amount(uint32_t amount) {
    double current_stock_price = stock_price();
    double total_cost = current_stock_price * amount;
    double available_cash = _tied_account->available_cash();

    if (current_stock_price == 0 || available_cash == 0) {
        return 0;
    }

    if (total_cost > available_cash) {//Still try to buy as many as possible within our available cash
        amount = available_cash / current_stock_price;
    }

    if (amount == 0) {
        return 0;
    }
    
    try {
        amount = PURCHASE_AMOUNT(_ticker, amount);
    }
    catch (std::exception &e){
        ASSERT(!"purchase_amount wrapper failed");
        amount = 0;
    }

    _known_stock_amount_owned = 0;//Just to make sure we didn't buy anything, double check the amount we have the next time we ask for stock_amount

    if (amount == 0) {//Didn't buy anything
        return 0;
    }

    return amount;
}


/*
Input:
Output: amount we BELIEVE we sold
Description:
Assumptions:
*/
uint32_t logical_ticker::sell_amount(uint32_t amount, bool force_sell/* = false*/) {
    uint_fast32_t already_held_amount = amount_owned();
    uint_fast32_t selling_amount = 0;
    uint_fast32_t need_amount;

    if (already_held_amount < amount) {
        amount = already_held_amount;
    }

    need_amount = amount;

    if (amount == 0) {
        return 0;
    }

    if (_transactions.size() < amount) {//We do not know the transaction history of some of these stocks, current behavior is to sell
        selling_amount = amount - _transactions.size();
    }

    for(std::list<list_insert*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {
        if (need_amount == 0) {
            break;
        }
        //Current price is lower than bought price
        if ((*it)->price > stock_price()) {
            if (!_can_sell_at_loss_default && !force_sell) {
                break;
            }
        }
        //Can move forward, we are either making a profit or profit doesn't matter
        if ((*it)->amount <= need_amount) {
            need_amount -= (*it)->amount;
            selling_amount += (*it)->amount;
        } else {
            selling_amount += need_amount;
            break;
        }
    }

    ASSERT(!_can_sell_at_loss_default || selling_amount == amount);//Saved amount isn't lining up with max requested amount

    if (selling_amount == 0) {
        return 0;
    }

    try {
        amount = SELL_AMOUNT(_ticker, selling_amount);
    }
    catch (std::exception &e){
        ASSERT(!"sell_amount wrapper failed");
        amount = 0;
    }

    _known_stock_amount_owned = 0;//Just to make sure we didn't sell anything, double check the amount we have the next time we ask for stock_amount

    //Launch a thread to check if we truly traded the requested amount after a short time period (~15? secs)
    //In the logical_account
    //We actually adjust the struct value in the thread
    return amount;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::modify_selling_at_loss(bool enabled) {
    _can_sell_at_loss_default = enabled;
}


/*
Input:
Output:
Description:
Assumptions:
*/
std::string logical_ticker::ticker() {
    return _ticker;
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
double logical_ticker::stock_price_at_time(int16_t requested_year/* = INT16_MIN*/, int16_t requested_month/* = INT16_MIN*/, int16_t requested_day/* = INT16_MIN*/, int16_t requested_hour/* = INT16_MIN*/, int16_t requested_minute/* = INT16_MIN*/, int16_t requested_second/* = INT16_MIN*/,
                                           int16_t year_deviation/* = 0*/, int16_t month_deviation/* = 0*/, int16_t day_deviation/* = 0*/, int16_t hour_deviation/* = 0*/, int16_t minute_deviation/* = 0*/, int16_t second_deviation/* = 0*/) {
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