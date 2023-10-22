#include <thread>
#include "logical_ticker.h"
#include "wrapper_api.h"
#include "assert_and_verify.h"


#define STOCK_PRICE_TIMEOUT 5 //Number of seconds we can use our stored stock price for a ticker before needing to check it again
#define STOCK_AMOUNT_TIMEOUT 21600 //Number of seconds we can use our stored stock amount before double checking (6 hours)


/*
Input:
Output: returns the amount owned 
Description: calls either _amount_owed_mine_func or _amount_owned_wrapper_func
Assumptions:
*/
uint32_t logical_ticker::amount_owned() {
    time_t current_time = time(NULL);

    if (_known_stock_amount_owned && ((current_time - _time_last_checked_amount) < STOCK_AMOUNT_TIMEOUT)) {
        ASSERT(!_known_stock_amount_owned_locked);//Make sure that the var is not locked
        return _amount_owned;
    }
    _time_last_checked_amount = current_time;
    //TODO Add discrepency detector/fixer if/when differences appear between list and variable
    _amount_owned = wrapper_amount_owned(_ticker);
    //If we do not know AND the value is not locked then do not make knwon_amount = true
    if (!_known_stock_amount_owned_locked) {
        _known_stock_amount_owned = 1;
    }

    return _amount_owned;
    
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
Output: amount we BELIEVE we purchased
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

    if (amount == 0) {
        return 0;
    }
    
    try {
        amount = wrapper_purchase_amount(_ticker, amount);
    }
    catch (std::exception &e){
        ASSERT(!("purchase_amount wrapper failed with exception e:",e.what()));
        amount = 0;
    }

    _known_stock_amount_owned = 0;//Just to make sure we didn't buy anything, double check the amount we have the next time we ask for stock_amount

    if (amount == 0) {//Didn't buy anything
        return 0;
    }

    _known_stock_amount_owned_locked = 1;//Locked the amount_owned func from swapping to known

    //Launch a thread to check if we truly traded the requested amount after a short time period (~15? secs)
    std::thread sep_thread (logical_ticker::_double_check_transaction, already_held_amount + amount, already_held_amount);
    //We actually adjust the struct value in the thread
    return amount;
}


/*
Input:
Output: amount we BELIEVE we sold
Description:
Assumptions:
*/
uint32_t logical_ticker::sell_amount(uint32_t amount) {
    uint32_t already_held_amount = amount_owned();
    uint32_t selling_amount = 0;
    uint32_t need_amount;
    list_insert * transaction = _transactions.back();

    if (already_held_amount < amount) {
        amount = already_held_amount;
    }

    need_amount = amount;

    if (amount == 0) {
        return 0;
    }

    for(std::list<list_insert*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {
        if (need_amount == 0) {
            break;
        }
        //Current price is lower than bought price
        if ((*it)->price > stock_price()) {
            if (!_can_sell_at_loss_default) {
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
        amount = wrapper_sell_amount(_ticker, selling_amount);
    }
    catch (std::exception &e){
        ASSERT(!("sell_amount wrapper failed with exception e:",e.what()));
        amount = 0;
    }

    _known_stock_amount_owned = 0;//Just to make sure we didn't sell anything, double check the amount we have the next time we ask for stock_amount

    if (amount == 0) {//Didn't sell anything
        return 0;
    }

    _known_stock_amount_owned_locked = 1;//Locked the amount_owned func from swapping to known
    
    //Launch a thread to check if we truly traded the requested amount after a short time period (~15? secs)
    std::thread sep_thread (logical_ticker::_double_check_transaction, already_held_amount - amount, already_held_amount);
    //We actually adjust the struct value in the thread
    return amount;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline void logical_ticker::modify_selling_at_loss(bool enabled) {
    _can_sell_at_loss_default = enabled;
}