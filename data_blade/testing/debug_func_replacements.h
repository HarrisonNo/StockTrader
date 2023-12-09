#include <string>
#include <map>

#ifndef DEBUG_FUNC_REPLACEMENTS
#define DEBUG_FUNC_REPLACEMENTS


//So far three types of debug funcs
//uint32_t debug_amount_owned_func(std::string)
//double debug_stock_price_func(std::string)
//double debug_account_cash_func()


uint32_t debug_amount_owned_ZERO(std::string ticker);
extern std::map<std::string, uint32_t> debug_amount_owned_GLOBAL_map;
uint32_t debug_amount_owned_GLOBAL(std::string ticker);
uint32_t debug_amount_owned_USER(std::string ticker);



double debug_stock_price_ZERO(std::string ticker);
extern std::map<std::string, double> debug_stock_price_GLOBAL_map;
double debug_stock_price_GLOBAL(std::string ticker);
double debug_stock_price_USER(std::string ticker);



double debug_account_cash_ZERO();
extern double debug_account_cash_GLOBAL_var;
double debug_account_cash_GLOBAL();
double debug_account_cash_USER();

































#endif//DEBUG_FUNC_REPLACEMENTS