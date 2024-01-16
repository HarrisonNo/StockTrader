#include <string>
#include <map>

#ifndef DEBUG_FUNC_REPLACEMENTS
#define DEBUG_FUNC_REPLACEMENTS


//So far five types of debug funcs
//uint32_t debug_amount_owned_func(std::string)
//double debug_stock_price_func(std::string)
//double debug_account_cash_func()
//uint32_t debug_purchase_amount_func(uint32_t)
//uint32_t debug_sell_amount_func(uint32_t)
//time_t debug_current_time()
//void debug_sleep_for()


//RESET_ALL_GLOBAL_VALUES is destructive, we have to ensure we are using DEBUG_API before we call this
#define RESET_ALL_GLOBAL_VALUES \
    debug_amount_owned_RESET_GLOBAL(); \
    debug_stock_price_RESET_GLOBAL(); \
    debug_account_cash_RESET_GLOBAL(); \
    debug_amount_owned_func = debug_amount_owned_GLOBAL; \
    debug_stock_price_func = debug_stock_price_GLOBAL; \
    debug_account_cash_func = debug_account_cash_GLOBAL; \
    debug_purchase_amount_func = debug_purchase_amount_REQUESTED; \
    debug_sell_amount_func = debug_sell_amount_REQUESTED; \
    debug_amount_owned_SET_NATURAL_CHANGING_GLOBAL(true); \
    debug_account_cash_SET_NATURAL_CHANGING_GLOBAL(true); \
    debug_current_time_RESET_GLOBAL(); \
    debug_current_time_func = debug_current_time_GLOBAL; \
    debug_sleep_for_func = debug_sleep_for_FAKE; \
    debug_current_time_SET_NATURAL_CHANGING_GLOBAL(true); \
    std::filesystem::remove_all(SAVED_TICKER_DIR); \
    std::filesystem::remove_all(SAVED_ACCOUNT_USERS_DIR); \
    debug_RESET_RNG(); \
    debug_api_failure_RESET_GLOBAL();

//amount_owned
//Return set global val
uint32_t debug_amount_owned_GLOBAL(std::string ticker);
void debug_amount_owned_SET_GLOBAL(std::string ticker, int64_t set_amount);
void debug_amount_owned_SET_NATURAL_CHANGING_GLOBAL(bool enable_changing);
void debug_amount_owned_RESET_GLOBAL();
//Make a cout call to the user to enter a specified number
uint32_t debug_amount_owned_USER(std::string ticker);


//stock_price
//Return set global val
double debug_stock_price_GLOBAL(std::string ticker);
void debug_stock_price_SET_GLOBAL(std::string ticker, double set_price);
void debug_stock_price_RESET_GLOBAL();
//Make a cout call to the user to enter a specified number
double debug_stock_price_USER(std::string ticker);


//account_cash
//Return set global val
double debug_account_cash_GLOBAL();
void debug_account_cash_SET_GLOBAL(double set_cash);
void debug_account_cash_SET_NATURAL_CHANGING_GLOBAL(bool enable_changing);
void debug_account_cash_RESET_GLOBAL();
//Make a cout call to the user to enter a specified number
double debug_account_cash_USER();


//purchase_amount
uint32_t debug_purchase_amount_GLOBAL(std::string ticker, uint32_t requested_amount);//TODO
uint32_t debug_purchase_amount_REQUESTED(std::string ticker, uint32_t requested_amount);


//sell_amount
uint32_t debug_sell_amount_GLOBAL(std::string ticker, uint32_t requested_amount);//TODO
uint32_t debug_sell_amount_REQUESTED(std::string ticker, uint32_t requested_amount);


//current_time
//Return set global val
time_t debug_current_time_GLOBAL();
void debug_current_time_SET_GLOBAL(time_t set_time);
void debug_current_time_RESET_GLOBAL();
void debug_current_time_RESET_ADDED_GLOBAL();
void debug_current_time_SET_NATURAL_CHANGING_GLOBAL(bool enable_changing);
//Returns the real time(NULL) value unaltered
time_t debug_current_time_REAL();



//RNG
uint64_t debug_RNG();
void debug_ADJUST_RNG(uint64_t addition);
void debug_RESET_RNG();
uint64_t debug_RNG_GENERATE(uint64_t max);
bool debug_RNG_GENERATE_ODDS(uint32_t positive_coefficient, uint32_t negative_cofficient);


//PRICE RNG
double debug_PRICE_RNG_generate_new_price(std::string ticker, uint32_t positive_coefficient, uint32_t negative_cofficient,
                                          double average_pos_percent_change, double max_pos_percent_change,
                                          double average_neg_percent_change, double max_neg_percent_change);


//sleep_for
//Pretends to sleep for specified time and increments current_time_GLOBAL to reflect pretend time spent sleeping
void debug_sleep_for_FAKE(time_t sleep_secs);
//Sleep_till can be created solely in logical_account as it can simply call sleep_for



//HISTORICAL PRICING
void debug_historical_price_RESET_GLOBAL();
uint32_t debug_historical_price_PROFITABLE_GLOBAL(std::string ticker, double price);



//API FAILURE
uint32_t debug_api_failure_GET_GLOBAL();
uint32_t debug_api_failure_SET_GLOBAL(uint32_t new_val);//Returns old val
void debug_api_failure_RESET_GLOBAL();
bool debug_api_failure_FAIL_GLOBAL();
bool debug_api_failure_HAS_FAILED_GLOBAL();//Have we explicitly failed the latest trade?
bool debug_api_failure_RESET_HAS_FAILED_GLOBAL();//Returns old val






















#endif//DEBUG_FUNC_REPLACEMENTS