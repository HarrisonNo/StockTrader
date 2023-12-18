#include <thread>
#include "logical_account.h"
#include "logical_ticker.h"



/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
inline logical_ticker * logical_account::_get_logical_ticker(std::string ticker) {
    if (auto search = _logical_tickers.find(ticker); search != _logical_tickers.end()) {
        return search->second;
    }
    //Did not find
    return NULL;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline logical_ticker * logical_account::_create_logical_ticker(std::string ticker) {
    logical_ticker * new_logical_ticker = new logical_ticker(ticker);
    _logical_tickers[ticker] = new_logical_ticker;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline logical_ticker * logical_account::_get_or_create_logical_ticker(std::string ticker) {
    logical_ticker * lt;
    lt = _get_logical_ticker(ticker);
    if (!lt) {
        lt = _create_logical_ticker(ticker);
    }
    return lt;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline key _generate_key(std::string ticker, uint32_t amount) {
    time_t current_time = time(NULL);
    
    key return_key = current_time + amount + ticker[0];

    return return_key;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_account::_async_buy_stock_wrapper(std::string ticker, uint32_t amount, key async_key) {
    //Get the async_return
    if (async_key != 0) {
        async_return * ar = _keyed_transactions[async_key];
        ar->return_value = buy_stock(ticker, amount);
        ar->has_return_value = true;
        //Deleting kli happens after the value is retrived from it
    } else {
        buy_stock(ticker, amount);
    }
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_account::_async_sell_stock_wrapper(std::string ticker, uint32_t amount, key async_key) {
    //Get the async_return
    //If key val is zero then it was never really generated
    if (async_key != 0) {
        async_return * ar = _keyed_transactions[async_key];
        ar->return_value = sell_stock(ticker, amount);
        ar->has_return_value = true;
    } else {
        sell_stock(ticker, amount);
    }
    //Deleting kli happens after the value is retrived from it
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_account::_async_stock_price_wrapper(std::string ticker, bool force_check) {
    //Get the async_return
    //If key val is zero then it was never really generated
    stock_price(ticker, force_check);
    //Deleting kli happens after the value is retrived from it
    return;
}


/*
Input:
Output:
Description: TODO save everything not related to a ticker here, fuck if I know what that should be yet though
Assumptions:
*/
inline void logical_account::_save_self() {
    return;
}