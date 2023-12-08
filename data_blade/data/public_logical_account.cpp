#include <thread>
#include "logical_account.h"
#include "assert_and_verify.h"
#include "debug_api_and_wrapper.h"

/*
Input:
Output:
Description:
Assumptions:
*/
logical_account::logical_account() {
    _number_of_projections = 0;
    _time_last_checked_cash = 0;
    //TODO add in loading saved ticker and other vals?
}


/*
Input:
Output:
Description:
Assumptions:
*/
uint32_t logical_account::buy_stock(std::string ticker, uint32_t amount) {
    double total_projected_cost, stock_price;
    uint_fast32_t amount_purchased, amount_initially_held;
    logical_ticker * lt = _get_or_create_logical_ticker(ticker);

    if (amount == 0) {
        return 0;
    }

    stock_price = lt->stock_price();
    amount_initially_held = lt->amount_owned();
    total_projected_cost = amount * stock_price;

    if (total_projected_cost > _projected_cash) {
        amount = _projected_cash / stock_price;
        if (amount == 0) {
            return 0;
        }
        total_projected_cost = amount * stock_price;
    }

    _cash_lock.lock();
    _projected_cash -= total_projected_cost;
    _number_of_projections++;
    _cash_lock.unlock();

    amount_purchased = lt->purchase_amount(amount);

    _cash_lock.lock();
    if (_number_of_projections) {//We may have just checked the available cash and reset projections
        _projected_cash += (amount - amount_purchased) * stock_price;//Refund any we didn't buy
        _number_of_projections--;
        _cash -= amount_purchased * stock_price;
    }
    _known_cash_amount = 0;
    _cash_lock.unlock();

    return amount_purchased;
}


/*
Input:
Output:
Description:
Assumptions:
*/
key logical_account::async_buy_stock(std::string ticker, uint32_t amount) {
    //Generate key
    key async_key = _generate_key(ticker, amount);

    //Store key in list
    keyed_list_insert * kli = new keyed_list_insert(async_key);
    _keyed_transactions.push_back(kli);//Have to do inline because user may immediately check on the status of the key

    //Start thread
    std::thread sep_thread (logical_account::_async_buy_stock_wrapper, ticker, amount, async_key);

    //Can just return key here
    return async_key;
}


/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
uint32_t logical_account::sell_stock(std::string ticker, uint32_t amount) {
    double total_projected_profit, stock_price;
    uint_fast32_t amount_sold, amount_initially_held;
    logical_ticker * lt = _get_or_create_logical_ticker(ticker);

    if (amount > lt->amount_owned()) {
        amount = lt->amount_owned();
    }
    if (amount == 0) {
        return 0;
    }

    amount_initially_held = lt->amount_owned();
    stock_price = lt->stock_price();
    total_projected_profit = amount * stock_price;

    _cash_lock.lock();
    _projected_cash += total_projected_profit;
    _number_of_projections++;
    _cash_lock.unlock();

    amount_sold = lt->sell_amount(amount);

    _cash_lock.lock();
    if (_number_of_projections) {//We may have just checked the available cash and reset projections
        _projected_cash -= (amount - amount_sold) * stock_price;//Subtract any we didn't sell
        _number_of_projections--;
        _cash += amount_sold * stock_price;
    }
    _known_cash_amount = 0;
    _cash_lock.unlock();

    return amount_sold;
}


/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
key logical_account::async_sell_stock(std::string ticker, uint32_t amount) {
    //Generate key
    key async_key = _generate_key(ticker, amount);

    //Store key in list
    keyed_list_insert * kli = new keyed_list_insert(async_key);
    _keyed_transactions.push_back(kli);//Have to do inline because user may immediately check on the status of the key

    //Start thread
    std::thread sep_thread (logical_account::_async_sell_stock_wrapper, ticker, amount, async_key);

    //Can just return key here
    return async_key;
}


/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
double logical_account::available_cash(bool force_check = false) {
    time_t current_time = time(NULL);
    double internal_cash;

    if (_known_cash_amount && !force_check && ((current_time - _time_last_checked_cash) > MAX_KNOWN_SEC_TIMEOUT)) {
        return _cash;
    }

    try {
        internal_cash = ACCOUNT_CASH;
    }
    catch (std::exception &e){
        ASSERT(!("amount_owned wrapper failed with exception e:",e.what()));
        _cash_lock.unlock();
        return 0;
    }

    _cash_lock.lock();
    _cash = internal_cash;
    _projected_cash = _cash;
    _known_cash_amount = 1;
    _time_last_checked_cash = current_time;
    _number_of_projections = 0;
    _cash_lock.unlock();

    //Also need other stipulations like if it has been enough time since last check etc
    return _cash;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline uint32_t logical_account::get_key_value(key requested_key, bool auto_delete_entry = true) {
    uint_fast32_t return_value = 0;
    keyed_list_insert * kli;

    for (std::list<keyed_list_insert*>::iterator it = _keyed_transactions.begin(); it != _keyed_transactions.end(); ++it) {
        kli = (*it);
        return_value = kli->return_value;
        if ((*it)->stored_key == requested_key && (*it)->has_return_value) {
            if (auto_delete_entry) {
                _keyed_transactions.erase(it);
                free(kli);
            }
            break;
        }
    }

    return return_value;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline uint32_t logical_account::wait_for_key_value(key requested_key, bool auto_delete_entry = true) {
    keyed_list_insert * kli = _get_kli_from_list(requested_key);
    uint_fast32_t iterations = 2;

    if (kli != NULL) {
        while(!(kli->has_return_value) && iterations < UINT16_MAX) {
            iterations *= 2;
            std::this_thread::sleep_for(std::chrono::milliseconds(iterations));//Highly inefficient, may increase from 5
        }
    }

    return kli->return_value;
}