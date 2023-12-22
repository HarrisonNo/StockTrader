#include <thread>
#include <exception>
#include <fstream>
#include "assert_and_verify.h"
#include "directory_file_saving.h"
#include "logical_ticker.h"

#define DOUBLE_CHECK_TRANSACTION_SLEEP 15 //Number of seconds we sleep before double checking to make sure a transaction 100% completed


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::_load_transactions() {
    int temp_amount, temp_price;
    std::ifstream transaction_file;
    list_insert * temp_list_insert;
    check_and_create_dirs(HISTORICAL_TICKER_DIR(_ticker));
    //Got to where historical data is stored (the balls)
    check_and_create_dirs(SAVED_ACCOUNT_TICKER_DIR(_tied_account->account_name(), _ticker));
    //We have now verified that the example path "save_info/users/HarrisonQuiring/AMD/transactions"
    transaction_file.open(SAVED_ACCOUNT_TRANSACTIONS_FILE(_tied_account->account_name(), _ticker), std::ios::in);
    while (transaction_file >> temp_amount >> temp_price) {
        temp_list_insert = _create_list_node(temp_amount, temp_price);
        _transactions.push_back(temp_list_insert);
        _transactions_list_stock_count += temp_amount;
    }
    transaction_file.close();
}

/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::_save_transactions() {
    //_load_transactions is called on startup of logical_ticker which checks for transactions dir existence, no need to double check
    std::ofstream transaction_file;
    if (!_known_stock_amount_owned) {
        amount_owned();//Call to update in core transactions
    }
    //Open with binary and truncate, binary removes the need/capability of catching shit like '\n' while truncate deletes the file if it already existed(possibly bad if we crashed previously?)
    transaction_file.open(SAVED_ACCOUNT_TRANSACTIONS_FILE(_tied_account->account_name(), _ticker), std::ios::out | std::ios::trunc);
    for(std::list<list_insert*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {
        transaction_file << (*it)->amount << "\t";
        transaction_file << (*it)->price << "\n";
    }
    transaction_file.close();
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::_modify_transaction_list(int64_t amount, double price/* = -1*/) {
    if (amount == 0) {
        return;
    }
    if (price == -1) {
        price = stock_price();
    }
    //Adding to list
    if (amount > 0) {
        bool inserted = false;

        //Need to insert ordered, no easy way without using std::set :(
        for (std::list<list_insert*>::iterator itr = _transactions.begin() ; itr != _transactions.end(); ++itr) {
            //If the itr we are looking at has a greater price than the one we have then insert here
            if ((*itr)->price > price) {
                _transactions.insert(itr, _create_list_node(amount, price));
                inserted = true;
                break;
            }
            //If a node in the list already has the exact same price then just increment that node's amount and be done with it
            else if ((*itr)->price == price) {
                (*itr)->amount += amount;
                inserted = true;
                break;
            }
        }
        //Didn't actually insert anywhere, just toss to back
        if (!inserted) {
            _transactions.push_back(_create_list_node(amount, price));
        }
        _transactions_list_stock_count += amount;
    }
    //Removing from list
    else {
        uint_fast32_t og_amount = amount;
        
        amount *= -1;//Amount must have been negative

        while (amount > 0 && !(_transactions.empty())) {
            list_insert * old_transaction = _transactions.back();
            bool sell_from_transaction = false;
            if (old_transaction->price > price) {
                if (_can_sell_at_loss_default) {
                    sell_from_transaction = true;
                }
            } else { //Can always sell here regardless of user request
                sell_from_transaction = true;
            }

            if (sell_from_transaction) {
                if (amount >= old_transaction->amount) {
                    amount -= old_transaction->amount;
                    _transactions.pop_back();//Remove node
                    _delete_list_node(old_transaction);//And delete it
                } else {
                    old_transaction->amount -= amount;
                    break;
                }
            } else {
                break;//Can go no further
            }
        }
        if (og_amount > _transactions_list_stock_count) {
            ASSERT(!"amount_sold is more than known transaction_count in _modify_transaction_list");
            _transactions_list_stock_count = 0;
        } else {
            _transactions_list_stock_count -= og_amount;
        }
    }
    _save_transactions();
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::_save_stock_price_at_time(double stock_price, time_t current_time/* = 0*/) {
    struct tm * time_info;
    std::fstream historical_price_file;
    int16_t temp_day, temp_hour, temp_minute, temp_second;
    double temp_price;
    std::streampos file_position;

    if (current_time == 0) {
        time(&current_time);
    }

    time_info = localtime(&current_time);
    check_and_create_dirs(HISTORICAL_TICKER_YEAR_DIR(_ticker, std::to_string(time_info->tm_year)));
    historical_price_file.open(HISTORICAL_TICKER_MONTH_FILE(_ticker, std::to_string(time_info->tm_year), std::to_string(time_info->tm_mon)), std::ios::in | std::ios::out);
    file_position = historical_price_file.tellg();//Stores postion of file at start, updated at end of every while loop

    //This breaks once we have gone right past where we want to insert
    //Insert after while loops finishes
    while (historical_price_file >> temp_day >> temp_hour >> temp_minute >> temp_second >> temp_price) {
        if (temp_day < time_info->tm_mday) {
            goto continue_while;
        }
        if (temp_day > time_info->tm_mday) {
            break;
        }
        if (temp_hour < time_info->tm_hour) {
            goto continue_while;
        }
        if (temp_hour > time_info->tm_hour) {
            break;
        }
        if (temp_minute < time_info->tm_min) {
            goto continue_while;
        }
        if (temp_minute > time_info->tm_min) {
            break;
        }
        if (temp_second < time_info->tm_sec) {
            goto continue_while;
        }
        if (temp_second > time_info->tm_sec) {
            break;
        }
        continue_while:
        file_position = historical_price_file.tellg();
    }
    historical_price_file.seekg(file_position);
    //Save to file at specified position
    historical_price_file << time_info->tm_mday << "\t" <<  time_info->tm_hour << "\t" << time_info->tm_min << "\t" << time_info->tm_sec << "\t" << stock_price << "\n";
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::_catch_invalid_dates(int16_t * year, int16_t * month, int16_t * day, int16_t * hour, int16_t * minute, int16_t * second) {
    time_t current_time;
    struct tm * time_info;

    time(&current_time);
    time_info = localtime(&current_time);
    if (*year == INT16_MAX || *year == INT16_MIN) {
        *year = time_info->tm_year;
    }
    if (*month == INT16_MAX || *month == INT16_MIN) {
        *month = time_info->tm_mon;
    }
    if (*day == INT16_MAX || *day == INT16_MIN) {
        *day = time_info->tm_mday;
    }
    if (*hour == INT16_MAX || *hour == INT16_MIN) {
        *hour = time_info->tm_hour;
    }
    if (*minute == INT16_MAX || *minute == INT16_MIN) {
        *minute = time_info->tm_min;
    }
    if (*second == INT16_MAX || *second == INT16_MIN) {
        *second = time_info->tm_sec;
    }
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::_date_corrections(int16_t * min_year, int16_t * min_month, int16_t * min_day, int16_t * min_hour, int16_t * min_minute, int16_t * min_second,
                              int16_t * max_year, int16_t * max_month, int16_t * max_day, int16_t * max_hour, int16_t * max_minute, int16_t * max_second) {
    //https://cplusplus.com/reference/ctime/tm/
    //requested_year = years since 1900
    //requested_month = months since january 0-11
    //requested_day = days of the month 1-31
    //reqeusted_hour = hours since midnight 0-23
    //requested_minute = minutes after the hour 0-59
    //requested_second = seconds after the minute 0-59
    uint8_t days_per_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    while (*max_second > 59) {
        *max_minute += 1;
        *max_second -= 60;
    }
    while (*min_second < 0) {
        *min_minute -= 1;
        *min_second += 60;
    }

    while (*max_minute > 59) {
        *max_hour += 1;
        *max_minute -= 60;
    }
    while (*min_minute < 0) {
        *min_hour -= 1;
        *min_minute += 60;
    }

    while (*max_hour > 23) {
        *max_day += 1;
        *max_hour -= 24;
    }
    while (*min_hour < 0) {
        *min_day -= 1;
        *min_hour += 24;
    }

    /* Need to catch potential over/underflow constantly */

    while (*max_month > 11) {
        *max_year += 1;
        *max_month -= 12;
    }
    while (*min_month < 0) {
        *min_year -= 1;
        *min_month +=12;
    }

    while (*max_day > days_per_month[*max_month]) {
        *max_day -= days_per_month[*max_month];
        *max_month += 1;
        while (*max_month > 11) {
            *max_year += 1;
            *max_month -= 12;
        }
    }
    while (*min_day < 0) {
        *min_day -= days_per_month[*min_month];
        *min_month -= 1;
        while (*min_month < 0) {
            *min_year -= 1;
            *min_month +=12;
        }
    }

    while (*max_month > 11) {
        *max_year += 1;
        *max_month -= 12;
    }
    while (*min_month < 0) {
        *min_year -= 1;
        *min_month +=12;
    }
}