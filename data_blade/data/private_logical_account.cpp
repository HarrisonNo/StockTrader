#include "logical_account.h"



/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
logical_ticker * logical_account::_have_ticker(std::string ticker) {
    logical_ticker * desired_ticker = NULL;

    for(std::list<logical_ticker*>::iterator it = _logical_tickers.begin(); it != _logical_tickers.end(); ++it) {
        if ((*it)->ticker() == ticker) {
            desired_ticker = (*it);
        }
    }

    return desired_ticker;
}