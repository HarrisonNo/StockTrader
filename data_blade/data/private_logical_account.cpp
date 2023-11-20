#include "logical_account.h"


/*
Input:
Output:
Description:
Assumptions:
*/
logical_account::logical_account() {
    _number_of_projections = 0;
}



/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
inline logical_ticker * logical_account::_get_logical_ticker(std::string ticker) {
    for(std::list<logical_ticker*>::iterator it = _logical_tickers.begin(); it != _logical_tickers.end(); ++it) {
        if ((*it)->ticker() == ticker) {
            return (*it);
        }
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
inline logical_ticker * _create_logical_ticker(std::string ticker) {
    logical_ticker * new_logical_ticker = new logical_ticker(ticker);
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
    //Get the keyed_list insert
    keyed_list_insert * kli = _get_kli_from_list(async_key);
    kli->return_value = buy_stock(ticker, amount);
    kli->has_return_value = true;
    //Deleting kli happens after the value is retrived from it
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
inline uint32_t logical_account::get_key_value(key requested_key, bool auto_delete_kli = true) {
    uint32_t return_value;
    keyed_list_insert * kli;

    if (auto_delete_kli) {
        for (std::list<keyed_list_insert*>::iterator it = _keyed_transactions.begin(); it != _keyed_transactions.end(); ++it) {
            kli = (*it);
            return_value = kli->return_value;
            if ((*it)->stored_key == requested_key && (*it)->has_return_value) {
                _keyed_transactions.erase(it);
                free(kli);
            }
        }
    }

    return return_value;
}