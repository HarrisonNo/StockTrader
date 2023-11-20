#include <thread>
#include "logical_account.h"
#include "wrapper_class.h"
#include "assert_and_verify.h"




/*
Input:
Output:
Description:
Assumptions:
*/
uint32_t logical_account::buy_stock(std::string ticker, uint32_t amount) {
    double total_projected_cost, stock_price;
    uint32_t amount_purchased;

    if (amount == 0) {
        return 0;
    }

    logical_ticker * lt = _get_or_create_logical_ticker(ticker);
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
    _projected_cash += (amount - amount_purchased) * stock_price;//Refund any we didn't buy
    if (_number_of_projections) {//We may have just checked the available cash and reset projections
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
    uint32_t stock_bought;
    key async_key = _generate_key(ticker, amount);

    //Store key in list
    keyed_list_insert * kli = _create_keyed_list_node(async_key);
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
    //TODO
}


/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
key logical_account::async_sell_stock(std::string ticker, uint32_t amount) {
    //TODO
}


/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
double logical_account::available_cash() {
    time_t current_time = time(NULL);
    if (!_known_cash_amount) {
        _number_of_projections = 0;

        try {
            _cash = get_wrapper_class()->wrapper_account_cash();
            _projected_cash = _cash;
            _known_cash_amount = 1;
        }
        catch (std::exception &e){
            ASSERT(!("amount_owned wrapper failed with exception e:",e.what()));
            _cash = 0;
        }
        _projected_cash = _cash;
    }
    if (_number_of_projections) {
        //Reset projections
        _number_of_projections = 0;
    }
    //Also need other stipulations like if it has been enough time since last check etc
    return _cash;
}