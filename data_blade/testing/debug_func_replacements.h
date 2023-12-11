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


uint32_t debug_amount_owned_ZERO(std::string ticker);
inline uint32_t debug_amount_owned_SET_GLOBAL(std::string ticker, uint32_t set_amount);
inline void debug_amount_owned_SET_NATURAL_CHANGING(bool enable_changing);
uint32_t debug_amount_owned_GLOBAL(std::string ticker);
uint32_t debug_amount_owned_USER(std::string ticker);



double debug_stock_price_ZERO(std::string ticker);
inline double debug_stock_price_SET_GLOBAL(std::string ticker, double set_price);
double debug_stock_price_GLOBAL(std::string ticker);
double debug_stock_price_USER(std::string ticker);



double debug_account_cash_ZERO();
inline double debug_account_cash_SET_GLOBAL(double set_cash);
double debug_account_cash_GLOBAL();
double debug_account_cash_USER();

//TODO IMPLEMENT
uint32_t debug_purchase_amount_ZERO(std::string ticker, uint32_t requested_amount);
uint32_t debug_purchase_amount_REQUESTED(std::string ticker, uint32_t requested_amount);


uint32_t debug_sell_amount_ZERO(std::string ticker, uint32_t requested_amount);
uint32_t debug_sell_amount_REQUESTED(std::string ticker, uint32_t requested_amount);































#endif//DEBUG_FUNC_REPLACEMENTS