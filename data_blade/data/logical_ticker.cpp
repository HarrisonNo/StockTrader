#include <exception>
#include <thread>
#include "logical_ticker.h"
#include "../wrapper/wrapper_api.h"
#include "../misc/asserts_and_verifies/assert_and_verify.h"

#define SWAP_AMOUNT_OWNED_TO_WRAPPER _amount_owned_func = &_amount_owned_wrapper_func//Swap to wrapper version, should swap back to my version the next time we check amount owned
#define SWAP_AMOUNT_OWNED_TO_MINE _amount_owned_func = &_amount_owned_mine_func //Swap to personal version, should swap back to wrapper verison if we buy/sell any

#define STOCK_PRICE_TIMEOUT 5 //Number of seconds we can use our stored stock price for a ticker before needing to check it again
#define DOUBLE_CHECK_TRANSACTION_SLEEP 15 //Number of seconds we sleep before double checking to make sure a transaction 100% completed
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
    } catch (std::exception &e) {
        ASSERT(!("amount_owned wrapper failed with exception e:",e.what()))
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
    uint32_t already_held_amount = amount_owned();
    double current_stock_price = stock_price();
    double total_cost = current_stock_price * amount;
    double available_cash = _tied_account->available_cash();

    if (total_cost > available_cash) {//Still try to buy as many as possible within our available cash
        amount = available_cash / current_stock_price;
    }

    if (amount <= 0) {
        return 0;
    }
    
    try {
        amount = wrapper_purchase_amount(_ticker, amount);
    }
    catch (std::exception &e){
        ASSERT(!("purchase_amount wrapper failed with exception e:",e.what()));
        amount = 0;
    }

    if (amount <= 0) {//Didn't buy anything
        return 0;
    }

    SWAP_AMOUNT_OWNED_TO_WRAPPER; //Swap to wrapper for amount owned as some problems may have actually caused us to not really buy anything

    //Launch a thread to check if we truly traded the requested amount after a short time period (~15? secs)
    std::thread sep_thread (logical_ticker::_double_check_transaction, already_held_amount + amount, amount, already_held_amount);
}


/*
Input:
Output: amount sold
Description:
Assumptions:
*/
uint32_t logical_ticker::sell_amount(uint32_t amount) {

}


/*
Input:
Output: bool did everything go according to plan
Description: double checks that a transaction 100% went through, then updates the various stats and lists accordingly as we only then consider it completed
Assumptions:
*/
bool logical_ticker::_double_check_transaction(uint32_t expected_held_amount, uint32_t attempted_to_trade_amount, uint32_t initial_held_amount) {
    int8_t success = -1;//Complete failure
    int64_t previous_differential = 0;
    int64_t differential = 0;

    for (int attempts = 1; attempts <= 3; attempts++) {//Attempt the following three times

        std::this_thread::sleep_for(std::chrono::seconds(DOUBLE_CHECK_TRANSACTION_SLEEP));//Immediately go to sleep to check everything
        uint32_t wrapper_amount = wrapper_amount_owned(_ticker);

        if (wrapper_amount == expected_held_amount) {//The transaction went through, we are cash money
            success = 1;//Complete success
            break;
        }

        //We don't have the expected amount past here

        if (wrapper_amount == initial_held_amount) {//We at least aren't partially off, go ahead and go for another attempt
            continue;
        }

        //We partially executed, not good

        //Calculate the partial execution
        differential = wrapper_amount - initial_held_amount;
        if (differential != previous_differential) {//If we technically executed on more shares than the last attempt go ahead for yet another attempt, potentially bypassing the 3-attempt system
            attempts--;
            previous_differential = differential;
        }
        success = 0;//Partial success, partial failure
        //Stick around for another attempt in case we do happen to execute some more of the initial order
    }

    //Finished with attempts
    if (success == 1) {//Total success

    }
    else if (success == -1) {//Total failure

    }
    else {//Partial success, partial failure

    }
}