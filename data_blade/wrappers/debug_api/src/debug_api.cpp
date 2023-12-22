#include <functional>
#include "debug_func_replacements.h"
#include "debug_api.h"


//Defaults to globals for all funcs
std::function<uint32_t(std::string)> debug_amount_owned_func = debug_amount_owned_GLOBAL;
std::function<double(std::string)> debug_stock_price_func = debug_stock_price_GLOBAL;
std::function<double()> debug_account_cash_func = debug_account_cash_GLOBAL;
std::function<uint32_t(std::string, uint32_t)> debug_purchase_amount_func = debug_purchase_amount_REQUESTED;
std::function<uint32_t(std::string, uint32_t)> debug_sell_amount_func = debug_sell_amount_REQUESTED;