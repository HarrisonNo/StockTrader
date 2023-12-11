#include <functional>
#include "debug_api.h"



std::function<uint32_t(std::string)> debug_amount_owned_func;
std::function<double(std::string)> debug_stock_price_func;
std::function<double()> debug_account_cash_func;
std::function<uint32_t(std::string, uint32_t)> debug_purchase_amount_func;
std::function<uint32_t(std::string, uint32_t)> debug_sell_amount_func;