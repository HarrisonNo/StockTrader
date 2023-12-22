#include <thread>
#include "logical_account.h"
#include "assert_and_verify.h"
#include "logical_ticker.h"


#define MAX_KNOWN_SEC_TIMEOUT 21600 //Max number of seconds we can use our stored known amount before double checking (6 hours)


/*
Input:
Output:
Description:
Assumptions:
*/
logical_account::logical_account(std::string account_name/* = "PLACEHOLDER"*/, bool load_existing/* = true*/) {
    _number_of_projections = 0;
    _time_last_checked_cash = 0;
    _account_name = account_name;
    //TODO add in loading saved ticker and other vals?
    if (!load_existing) {
        //Purge any and all existing files which may have been previously created
        _purge_all_saved_info();
    } else {

    }
}


/*
Input:
Output:
Description: TODO destructing time
Assumptions:
*/
logical_account::~logical_account() {
    //Save ourselves
    _save_self();
    //Tickers will automatically be saved on being freed as the destructor auto saves each
    for (auto it = _logical_tickers.begin(); it != _logical_tickers.end(); it++) {
        //Call delete to trigger destructor
        logical_ticker * lt = it->second;
        _logical_tickers.erase(it);
        delete(lt);
    }
    //Then delete all as we are falling out of scope
}


/*
Input:
Output:
Description:
Assumptions:
*/
std::string logical_account::account_name() {
    return _account_name;
}


/*
Input:
Output:
Description: TODO destructing time
Assumptions:
*/
void logical_account::trigger_mass_save() {
    //Save ourselves
    _save_self();
    //Save each of our tickers
    for (auto it = _logical_tickers.begin(); it != _logical_tickers.end(); it++) {
        it->second->save_self();
    }
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
uint32_t logical_account::buy_stock(std::string ticker, uint32_t amount) {
    double total_projected_cost, stock_price;
    uint_fast32_t amount_purchased;
    logical_ticker * lt = _get_or_create_logical_ticker(ticker);

    if (amount == 0) {
        return 0;
    }

    stock_price = lt->stock_price();
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
std::future<uint32_t> logical_account::async_buy_stock(std::string ticker, uint32_t amount) {
    return std::async (logical_account::buy_stock, this, ticker, amount);
}


/*
Input:
Output:
Description:
Assumptions:
*/
uint32_t logical_account::held_stock(std::string ticker, bool force_check/* = false*/) {
    logical_ticker * lt = _get_or_create_logical_ticker(ticker);
    return lt->amount_owned(force_check);
}


/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
uint32_t logical_account::sell_stock(std::string ticker, uint32_t amount, bool force_sell/* = false*/) {
    double total_projected_profit, stock_price;
    uint_fast32_t amount_sold, amount_initially_held;
    logical_ticker * lt = _get_or_create_logical_ticker(ticker);

    amount_initially_held = lt->amount_owned();

    if (amount > amount_initially_held) {
        amount = amount_initially_held;
    }
    if (amount == 0) {
        return 0;
    }
    
    stock_price = lt->stock_price();
    total_projected_profit = amount * stock_price;

    _cash_lock.lock();
    _projected_cash += total_projected_profit;
    _number_of_projections++;
    _cash_lock.unlock();

    amount_sold = lt->sell_amount(amount, force_sell);

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
std::future<uint32_t> logical_account::async_sell_stock(std::string ticker, uint32_t amount, bool force_sell/* = false*/) {
    return std::async(&logical_account::sell_stock, this, ticker, amount, force_sell);
}


/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
double logical_account::available_cash(bool force_check/* = false*/) {
    time_t current_time = time(NULL);
    double internal_cash;

    if (_known_cash_amount && !force_check && ((current_time - _time_last_checked_cash) > MAX_KNOWN_SEC_TIMEOUT)) {
        return _cash;
    }

    try {
        internal_cash = ACCOUNT_CASH;
    }
    catch (std::exception &e){
        ASSERT(!"amount_owned wrapper failed");
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
double logical_account::stock_price(std::string ticker, bool force_check/* = false*/) {
    logical_ticker * lt = _get_or_create_logical_ticker(ticker);
    return lt->stock_price(force_check);
}


/*
Input:
Output:
Description:
Assumptions:
*/
std::future<double> logical_account::async_stock_price(std::string ticker, bool force_check/* = false*/) {
    return std::async (logical_account::stock_price, this, ticker, force_check);
}