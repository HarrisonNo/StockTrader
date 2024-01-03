#include <thread>
#include <fstream>
#include <optional>
#include "logical_ticker.h"
#include "assert_and_verify.h"
#include "directory_file_saving.h"


#define MAX_STOCK_PRICE_TIMEOUT 2 //Number of seconds we can use our stored stock price for a ticker before needing to check it again
#define MAX_KNOWN_SEC_TIMEOUT 21600 //Max number of seconds we can use our stored known amount before double checking (6 hours)
#define MIN_KNOWN_SEC_TIMEOUT 60 //Min number of seconds we have to double check before we can use our stored known amount

/*
Input:
Output:
Description:
Assumptions:
*/
logical_ticker::logical_ticker(std::string input_ticker, logical_account * tied_account, bool allow_boot_loading/* = true*/) {
    _time_last_checked_price = 0;
    _time_last_checked_amount = 0;
    _time_last_executed_transaction = 0;
    _known_stock_amount_owned = 0;
    _ticker = input_ticker;
    _can_sell_at_loss_default = 0;
    _transactions_list_stock_count = 0;
    _loaded_month = INT_MAX;
    _loaded_year = INT_MAX;
    _tied_account = tied_account;
    _stock_price = 0;
    if (allow_boot_loading) {
        _load_transactions();
    }
    amount_owned(true);
}


/*
Input:
Output:
Description: Save and clear new-ed memory, so far all we need to do is save, should this be optional? TODO
Assumptions:
*/
logical_ticker::~logical_ticker() {
    //Save ourselves
    save_self();
    //Then delete all new-ed memory
    while(!(_transactions.empty())) {
        list_insert * li = _transactions.front();
        _transactions.pop_front();
        delete(li);
    }
    
}


/*
Input:
Output:
Description: Save everything, so far just need to save transactions
Assumptions:
*/
void logical_ticker::save_self() {
    _save_transactions();
}


/*
Input:
Output: returns the amount owned, 0 on wrapper failure(!!!!!!)
Description: returns amount owned, minimizing wrapper calls if we already checked it recently 
Assumptions:
*/
uint32_t logical_ticker::amount_owned(bool force_check/* = false*/) {
    time_t current_time = CURRENT_TIME();
    uint_fast32_t discrepancy, returned_amount_owned;

    //If:
    //We are not forcing a wrapper call
    //We know the stock amount
    //Has not been more than MAX_KNOWN_SEC_TIMEOUT since the last time we checked the amount
    //Has at least been more than MIN_KNOWN_SEC_TIMEOUT since the last time we made a transaction
    //Then: return the internal value for amount_owned
    if (!force_check && _known_stock_amount_owned && ((current_time - _time_last_checked_amount) < MAX_KNOWN_SEC_TIMEOUT) && ((current_time - _time_last_executed_transaction) > MIN_KNOWN_SEC_TIMEOUT)) {
        return _amount_owned;
    }

    try {
        returned_amount_owned = AMOUNT_OWNED_CALL(_ticker);
    }
    catch (std::exception &e) {
        ASSERT(!"amount_owned wrapper failed");
         _known_stock_amount_owned = 0;
        return 0;
    }

    _amount_owned = returned_amount_owned;
    _time_last_checked_amount = current_time;
    discrepancy = _transactions_list_stock_count - _amount_owned;
    //Catch discrepancy with transactions
    if (discrepancy != 0) {
        _modify_transaction_list(_amount_owned - _transactions_list_stock_count);
        _known_stock_amount_owned = 0;//We had to modify the list, we may not know the real amount
    } else {
        _known_stock_amount_owned = 1;
    }

    return _amount_owned;
}


/*
Input:
Output: current stock price, 0 on wrapper failure(!!!!!!)
Description: gets the stock price, minimizing wrapper calls if we already checked it recently 
Assumptions:
*/
double logical_ticker::stock_price(bool force_check/* = false*/) {
    time_t current_time = CURRENT_TIME();

    if (!force_check && (current_time - _time_last_checked_price) < MAX_STOCK_PRICE_TIMEOUT) {
        return _stock_price;
    }

    try {
        _stock_price = STOCK_PRICE_CALL(_ticker);
    }
    catch (std::exception &e) {
        ASSERT(!"stock_price wrapper failed");
        //_trigger_wrapper_failure();//Increments var and checks if enough failures have occured within X seconds, if so then trigger a clean shutdown //TODO
        return 0;
    }

    _time_last_checked_price = current_time;
    _save_stock_price_at_time(current_time);

    return _stock_price;
}


/*
Input:
Output: amount we BELIEVE we purchased
Description:
Assumptions:
*/
uint32_t logical_ticker::purchase_amount(uint32_t amount) {
    double current_stock_price = stock_price();
    double total_cost = current_stock_price * amount;
    double available_cash = _tied_account->available_cash();

    if (current_stock_price == 0 || available_cash == 0) {
        return 0;
    }

    if (total_cost > available_cash) {//Still try to buy as many as possible within our available cash
        amount = available_cash / current_stock_price;
    }

    if (amount == 0) {
        return 0;
    }
    
    try {
        amount = PURCHASE_AMOUNT(_ticker, amount);
    }
    catch (std::exception &e){
        ASSERT(!"purchase_amount wrapper failed");
        amount = 0;
    }

    _known_stock_amount_owned = 0;//Just to make sure we didn't buy anything, double check the amount we have the next time we ask for stock_amount

    if (amount == 0) {//Didn't buy anything
        return 0;
    }

    return amount;
}


/*
Input:
Output: amount we BELIEVE we sold
Description:
Assumptions:
*/
uint32_t logical_ticker::sell_amount(uint32_t amount, bool force_sell/* = false*/) {
    uint_fast32_t already_held_amount = amount_owned();
    uint_fast32_t selling_amount = 0;
    uint_fast32_t need_amount;

    if (already_held_amount < amount) {
        amount = already_held_amount;
    }

    need_amount = amount;

    if (amount == 0) {
        return 0;
    }

    if (_transactions_list_stock_count < amount) {//We do not know the transaction history of some of these stocks, current behavior is to sell
        selling_amount = amount - _transactions_list_stock_count;
    }

    for(std::list<list_insert*>::iterator it = _transactions.begin(); it != _transactions.end(); ++it) {
        if (need_amount == 0) {
            break;
        }
        //Current price is lower than bought price
        if ((*it)->price > stock_price()) {
            if (!_can_sell_at_loss_default && !force_sell) {
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
        amount = SELL_AMOUNT(_ticker, selling_amount);
    }
    catch (std::exception &e){
        ASSERT(!"sell_amount wrapper failed");
        amount = 0;
    }

    _known_stock_amount_owned = 0;//Just to make sure we didn't sell anything, double check the amount we have the next time we ask for stock_amount

    //Launch a thread to check if we truly traded the requested amount after a short time period (~15? secs)
    //In the logical_account
    //We actually adjust the struct value in the thread
    return amount;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_ticker::modify_selling_at_loss(bool enabled) {
    _can_sell_at_loss_default = enabled;
}


/*
Input:
Output:
Description:
Assumptions:
*/
std::string logical_ticker::ticker() {
    return _ticker;
}


/*
Input:
Output:
Description: This could 100% be optimized by storing start and end times and simply correcting our held vector, TODO
Assumptions:
*/
std::vector<std::pair<time_t, double>> * logical_ticker::load_historical_prices(time_t start_time, time_t end_time) {
    struct tm * start_time_info = localtime(&start_time);
    struct tm * end_time_info = localtime(&end_time);
    _historical_prices_ranged.clear();
    while (start_time_info->tm_year < end_time_info->tm_year || ((start_time_info->tm_mon <= end_time_info->tm_mon) && start_time_info->tm_year == end_time_info->tm_year)) {
        _load_historical_price_file(start_time_info->tm_mon, start_time_info->tm_year);
        for (std::pair<time_t, double> pr : _historical_prices_month_file) {
            if (pr.first >= start_time && pr.first <= end_time) {
                _historical_prices_ranged.push_back(pr);
            }
        }
        /* End */
        start_time_info->tm_mon++;
        time_t temp_time = mktime(start_time_info);
        start_time_info = localtime(&temp_time);
    }
    return &_historical_prices_ranged;
}