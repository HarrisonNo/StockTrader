#include <string>
#include "global_defines.h"

#ifndef DIRECTORY_FILE_SAVING
#define DIRECTORY_FILE_SAVING


#define GLOBAL_SAVED_INFO_DIR "saved_info/"

//Some pathways may change based on debug/non-debug api
#if DEBUG_API
//Dont want to override actual historical prices and user info with fabricated ones
#define SAVED_TICKER_DIR            GLOBAL_SAVED_INFO_DIR "DEBUG_historical_ticker_info/"
#define SAVED_ACCOUNT_USERS_DIR     GLOBAL_SAVED_INFO_DIR "DEBUG_users/"
#else
#define SAVED_TICKER_DIR            GLOBAL_SAVED_INFO_DIR "historical_ticker_info/"
#define SAVED_ACCOUNT_USERS_DIR     GLOBAL_SAVED_INFO_DIR "users/"
#endif


#define HISTORICAL_TICKER_DIR(ticker)                       SAVED_TICKER_DIR + ticker + "/"
#define HISTORICAL_TICKER_YEAR_DIR(ticker, year)            HISTORICAL_TICKER_DIR(ticker) + year + "/"
#define HISTORICAL_TICKER_MONTH_FILE(ticker, year, month)   HISTORICAL_TICKER_YEAR_DIR(ticker, year) + month

#define SAVED_ACCOUNT_DIR(account)                          SAVED_ACCOUNT_USERS_DIR + account + "/"
#define SAVED_ACCOUNT_TICKER_DIR(account, ticker)           SAVED_ACCOUNT_DIR(account) + ticker + "/"
#define SAVED_ACCOUNT_TRANSACTIONS_FILE(account, ticker)    SAVED_ACCOUNT_TICKER_DIR(account, ticker) + "transactions_file"


bool check_and_create_dirs(std::string path);






























#endif