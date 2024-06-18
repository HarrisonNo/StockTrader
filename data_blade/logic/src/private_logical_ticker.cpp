#include <thread>
#include <exception>
#include <fstream>
#include <climits>
#include "assert_and_verify.h"
#include "directory_file_saving.h"
#include "logical_ticker.h"


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::_load_transactions() {
    int temp_amount, temp_price;
    std::ifstream transaction_file;
    check_and_create_dirs(HISTORICAL_TICKER_DIR(_ticker));
    //Got to where historical data is stored (the balls)
    check_and_create_dirs(SAVED_ACCOUNT_TICKER_DIR(_tied_account->account_name(), _ticker));
    //We have now verified that the example path "save_info/users/HarrisonQuiring/AMD/transactions"
    transaction_file.open(SAVED_ACCOUNT_TRANSACTIONS_FILE(_tied_account->account_name(), _ticker), std::ios::in);
    while (transaction_file >> temp_amount >> temp_price) {
        _transactions.push_back(std::make_pair(temp_price, temp_amount));
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
    //Open with binary and truncate, binary removes the need/capability of catching shit like '\n' while truncate deletes the file if it already existed(possibly bad if we crashed previously?)
    transaction_file.open(SAVED_ACCOUNT_TRANSACTIONS_FILE(_tied_account->account_name(), _ticker), std::ios::out | std::ios::trunc);
    for(auto it: _transactions) {
        transaction_file << it.second << "\t";
        transaction_file << it.first << "\n";
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
    uint_fast32_t og_amount = abs(amount);

    if (amount == 0) {
        return;
    }
    if (price == -1) {
        price = stock_price();
    }

    //Adding to list
    if (amount > 0) {
        bool inserted = false;

        for (auto it: _transactions) {
            if (it.first == price) {
                it.second += amount;
                inserted = true;
                break;
            }
        }

        if (!inserted) {
            _transactions.emplace_back(price, amount);
            sort(_transactions.begin(), _transactions.end());//I'm lazy, sue me
        }

        _transactions_list_stock_count += amount;
    }
    //Removing from list
    else {
        amount *= -1;
        for (auto it = _transactions.begin(); it != _transactions.end(); /*Intentionally blank*/) {
            if ((*it).second > amount) {
                (*it).second -= amount;
                break;
            } else {
                amount -= (*it).second;
                it = _transactions.erase(it);
                if (amount == 0) {
                    break;
                }
            }
        }
        _transactions_list_stock_count -= og_amount;
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

    if (current_time == 0) {
        current_time = CURRENT_TIME();
    }

    time_info = localtime(&current_time);
    //Load the vector
    if (!_load_historical_price_file(time_info->tm_mon, time_info->tm_year)) {
        return;
    }
    std::pair<time_t, double> new_pair = std::make_pair(current_time, stock_price);
    //This is absolutely cursed, insert into the vector ordered, with O(logN)
    _historical_prices_month_file.insert(std::upper_bound(_historical_prices_month_file.begin(), _historical_prices_month_file.end(), new_pair), new_pair);
    //Open month file with trunc to delete existing contents
    check_and_create_file(&historical_price_file, std::ios::in | std::ios::out | std::ios::trunc, HISTORICAL_TICKER_MONTH_FILE(_ticker, std::to_string(time_info->tm_year), std::to_string(time_info->tm_mon)));
    for (std::pair<time_t, double> pr : _historical_prices_month_file) {
        historical_price_file << pr.first << "\t" <<pr.second<< "\n";
    }
    historical_price_file.close();
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
bool logical_ticker::_load_historical_price_file(int month/* = INT_MAX*/, int year/* = INT_MAX*/) {
    std::fstream historical_price_file;
    double temp_price;
    time_t temp_time, current_time = CURRENT_TIME();
    struct tm * time_info = localtime(&current_time);

    if (month == INT_MAX) {
        month = time_info->tm_mon;
    }
    if (year == INT_MAX) {
        year = time_info->tm_year;
    }
    
    if (_loaded_year == year && _loaded_month == month) {
        return true;//No need to load anything
    }

    //Read in specific file
    
    if (!check_and_create_dirs(HISTORICAL_TICKER_YEAR_DIR(_ticker, std::to_string(year))) ||
        !check_and_create_file(&historical_price_file, std::ios::in | std::ios::out, HISTORICAL_TICKER_MONTH_FILE(_ticker, std::to_string(year), std::to_string(month)))) {
        return false;
    }

    _loaded_year = year;
    _loaded_month = month;
    _historical_prices_month_file.clear();

    while(historical_price_file >> temp_time >> temp_price) {
        _historical_prices_month_file.emplace_back(temp_time, temp_price);
    }

    historical_price_file.close();
    return true;
}