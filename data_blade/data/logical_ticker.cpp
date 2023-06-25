#include <exception>
#include "logical_ticker.h"
#include "../wrapper/wrapper_api.h"
#include "../misc/asserts_and_verifies/assert_and_verify.h"

#define SWAP_AMOUNT_OWNED_TO_WRAPPER _amount_owned_func = &_amount_owned_wrapper_func//Swap to wrapper version, should swap back to my version the next time we check amount owned
#define SWAP_AMOUNT_OWNED_TO_MINE _amount_owned_func = &_amount_owned_mine_func //Swap to personal version, should swap back to wrapper verison if we buy/sell any

#define STOCK_PRICE_TIMEOUT 5 //Number of seconds we can use our stored stock price for a ticker before needing to check it again
/*
Input:
Output:
Description:
Assumptions:
*/
logical_ticker::logical_ticker(std::string input_ticker) {
    _time_last_checked_price = 0;
    _ticker = input_ticker;
    SWAP_AMOUNT_OWNED_TO_WRAPPER;//Inits the amount_owned func to the wrapper function, needs to switch to personal version when get the amount_owned
}


/*
Input:
Output: returns the amount owned 
Description: calls either _amount_owed_mine_func or _amount_owned_wrapper_func
Assumptions:
*/
inline uint32_t logical_ticker::amount_owned() {
    return _amount_owned_func();
}


/*
Input:
Output:
Description: one of two functions used in amount_owned
Assumptions:
*/
inline uint32_t logical_ticker::_amount_owned_mine_func() {
    return _amount_owned;
}


/*
Input:
Output:
Description: one of two functions used in amount_owned
Assumptions:
*/
uint32_t logical_ticker::_amount_owned_wrapper_func() {
    uint32_t amount;

    try {
        amount = wrapper_amount_owned(_ticker);
    } catch (std::exception e) {
        ASSERT(!"amount_owned wrapper failed")
        amount = 0;
    }

    SWAP_AMOUNT_OWNED_TO_MINE;//Swap to mine as we have the most up-to-date stats

    return amount;
}


/*
Input:
Output: current stock price
Description: gets the stock price, minimizing wrapper calls if we already checked it recently 
Assumptions:
*/
double logical_ticker::stock_price() {
    time_t current_time = time(NULL);

    if ((current_time - _time_last_checked_price) > STOCK_PRICE_TIMEOUT) {
        _stock_price = wrapper_stock_price(_ticker);
        _time_last_checked_price = current_time;
    }

    return _stock_price;
}


/*
Input:
Output: amount purchased
Description:
Assumptions:
*/
uint32_t logical_ticker::purchase_amount(uint32_t amount) {

}


/*
Input:
Output: amount sold
Description:
Assumptions:
*/
uint32_t logical_ticker::sell_amount(uint32_t amount) {

}