#include <thread>
#include <filesystem>
#include "logical_account.h"
#include "logical_ticker.h"
#include "directory_file_saving.h"



/*
Input:
Output: returns the logical ticker if it exists, NULL if otherwise
Description:
Assumptions:
*/
logical_ticker * logical_account::_get_logical_ticker(std::string ticker) {
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
logical_ticker * logical_account::_create_logical_ticker(std::string ticker) {
    logical_ticker * new_logical_ticker = new logical_ticker(ticker, this);
    _logical_tickers[ticker] = new_logical_ticker;
    return new_logical_ticker;
}


/*
Input:
Output:
Description:
Assumptions:
*/
logical_ticker * logical_account::_get_or_create_logical_ticker(std::string ticker) {
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
Description: TODO save everything not related to a ticker here, fuck if I know what that should be yet though
Assumptions:
*/
void logical_account::_save_self() {
    return;
}


/*
Input:
Output:
Description:
Assumptions:
*/
void logical_account::_purge_all_saved_info() {
    std::filesystem::remove_all(SAVED_ACCOUNT_DIR(account_name()));
}