#include <thread>
#include <exception>
#include <fstream>
#include "wrapper_api.h"
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
logical_ticker::logical_ticker(std::string input_ticker) {
    _time_last_checked_price = 0;
    _time_last_checked_amount = 0;
    _known_stock_amount_owned = 0;
    _known_stock_amount_owned_locked = 0;
    _ticker = input_ticker;
    _can_sell_at_loss_default = 0;
    _load_transactions();
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline
void logical_ticker::_load_transactions() {
    int temp_amount, temp_price;
    std::ifstream transaction_file;
    list_insert * temp_list_insert;
    check_and_create_dirs("saved_info/historical_ticker_info/" + _ticker);
    //Got to where historical data is stored (the balls)
    check_and_create_dirs("/saved_info/users/" + _tied_account->account_name() + "/" + _ticker);
    //We have now verified that the example path "save_info/users/HarrisonQuiring/AMD/transactions"
    transaction_file.open("/saved_info/users/" + _tied_account->account_name() + "/" + _ticker + "/transactions_file", std::ios::in);
    while (transaction_file >> temp_amount >> temp_price) {
        temp_list_insert = _create_list_node(temp_amount, temp_price);
        _transactions.push_back(temp_list_insert);
    }
    transaction_file.close();
}

/*
Input:
Output:
Description:
Assumptions:
*/
inline
void logical_ticker::_save_transactions() {
    //_load_transactions is called on startup of logical_ticker which checks for transactions dir existence, no need to double check
    std::ofstream transaction_file;
    //Open with binary and truncate, binary removes the need/capability of catching shit like '\n' while truncate deletes the file if it already existed(possibly bad if we crashed previously?)
    transaction_file.open("/saved_info/users/" + _tied_account->account_name() + "/" + _ticker + "/transactions_file", std::ios::out | std::ios::trunc);
    for(std::list<list_insert*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {
        transaction_file << (*it)->amount << "\t";
        transaction_file << (*it)->price << "\n";
    }
    transaction_file.close();
}


/*
Input:
Output: bool did everything go according to plan
Description: double checks that a transaction 100% went through, then updates the various stats and lists accordingly as we only then consider it completed
Assumptions:
*/
bool logical_ticker::_double_check_transaction(uint32_t expected_held_amount, uint32_t initial_held_amount) {
    int8_t success = -1;//Complete failure
    int64_t previous_differential = 0;
    int64_t differential = 0;

    ASSERT(!_known_stock_amount_owned && _known_stock_amount_owned_locked);//Should have been set to 0 and locked prior to entering here

    for (uint8_t attempts = 1; attempts <= 3; attempts++) {//Attempt the following three times

        std::this_thread::sleep_for(std::chrono::seconds(DOUBLE_CHECK_TRANSACTION_SLEEP));//Immediately go to sleep to check everything
        _amount_owned = wrapper_amount_owned(_ticker);

        if (_amount_owned == expected_held_amount) {//The transaction went through, we are cash money
            success = 1;//Complete success
            differential = _amount_owned - initial_held_amount;
            break;
        }

        //We don't have the expected amount past here

        if (_amount_owned == initial_held_amount) {//We at least aren't partially off, go ahead and go for another attempt
            continue;
        }

        //We partially executed, not good
        //Calculate the partial execution
        differential = _amount_owned - initial_held_amount;
        if (differential != previous_differential) {//If we technically executed on more shares than the last attempt go ahead for yet another attempt, potentially bypassing the 3-attempt system
            attempts--;
            previous_differential = differential;
        }
        success = 0;//Partial success, partial failure
        //Stick around for another attempt in case we do happen to execute some more of the initial order
    }

    _known_stock_amount_owned_locked = 0;//Unlock the value, we have gone as far as reasonably possible
    _modify_transaction_list(differential);

    //Finished with attempts
    if (success == 1) {//Total success
        _known_stock_amount_owned = 1;//We definitively know the amount of stock we have
        return true;
    }
    //Total failure OR Partial success, partial failure
    return false;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline
void logical_ticker::_modify_transaction_list(int32_t amount, double price = -1) {
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
    }
    //Removing from list
    else {
        amount *= -1;//Amount must have been negative
        while (amount > 0 && !(_transactions.empty())) {
            list_insert * old_transaction = _transactions.front();
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
                    _transactions.pop_front();//Remove node
                    _delete_list_node(old_transaction);//And delete it
                } else {
                    old_transaction->amount -= amount;
                    break;
                }
            } else {
                break;//Can go no further
            }
        }
    }
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline
void logical_ticker::_save_stock_price_at_time(double stock_price, time_t current_time = 0) {
    struct tm * time_info;
    std::fstream historical_price_file;
    int16_t temp_day, temp_hour, temp_minute, temp_second;
    double temp_price;
    std::streampos file_position;

    if (current_time == 0) {
        time(&current_time);
    }

    time_info = localtime(&current_time);
    check_and_create_dirs("saved_info/historical_ticker_info/" + _ticker + "/" + std::to_string(time_info->tm_year));
    historical_price_file.open("saved_info/historical_ticker_info/" + _ticker + "/" + std::to_string(time_info->tm_year) + "/" + std::to_string(time_info->tm_mon), std::ios::in | std::ios::out);
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