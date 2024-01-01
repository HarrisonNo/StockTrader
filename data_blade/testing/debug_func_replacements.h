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

#define RESET_ALL_GLOBAL_VALUES \
    debug_amount_owned_RESET_GLOBAL(); \
    debug_stock_price_RESET_GLOBAL(); \
    debug_account_cash_RESET_GLOBAL(); \
    debug_amount_owned_func = debug_amount_owned_GLOBAL; \
    debug_stock_price_func = debug_stock_price_GLOBAL; \
    debug_account_cash_func = debug_account_cash_GLOBAL; \
    debug_purchase_amount_func = debug_purchase_amount_REQUESTED; \
    debug_sell_amount_func = debug_sell_amount_REQUESTED; \
    debug_amount_owned_SET_NATURAL_CHANGING(true); \
    debug_account_cash_SET_NATURAL_CHANGING(true);


void debug_amount_owned_SET_GLOBAL(std::string ticker, uint32_t set_amount);
void debug_amount_owned_SET_NATURAL_CHANGING(bool enable_changing);
void debug_amount_owned_RESET_GLOBAL();
uint32_t debug_amount_owned_GLOBAL(std::string ticker);
uint32_t debug_amount_owned_USER(std::string ticker);



void debug_stock_price_SET_GLOBAL(std::string ticker, double set_price);
void debug_stock_price_RESET_GLOBAL();
double debug_stock_price_GLOBAL(std::string ticker);
double debug_stock_price_USER(std::string ticker);



void debug_account_cash_SET_GLOBAL(double set_cash);
void debug_account_cash_SET_NATURAL_CHANGING(bool enable_changing);
void debug_account_cash_RESET_GLOBAL();
double debug_account_cash_GLOBAL();
double debug_account_cash_USER();

//TODO IMPLEMENT
uint32_t debug_purchase_amount_GLOBAL(std::string ticker, uint32_t requested_amount);//TODO
uint32_t debug_purchase_amount_REQUESTED(std::string ticker, uint32_t requested_amount);


uint32_t debug_sell_amount_GLOBAL(std::string ticker, uint32_t requested_amount);//TODO
uint32_t debug_sell_amount_REQUESTED(std::string ticker, uint32_t requested_amount);































#endif//DEBUG_FUNC_REPLACEMENTS