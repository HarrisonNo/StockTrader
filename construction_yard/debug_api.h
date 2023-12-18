#include <string>
#include <functional>

#ifndef DEBUG_API_FILE
#define DEBUG_API_FILE




extern std::function<uint32_t(std::string)> debug_amount_owned_func;
extern std::function<double(std::string)> debug_stock_price_func;
extern std::function<double()> debug_account_cash_func;
extern std::function<uint32_t(std::string, uint32_t)> debug_purchase_amount_func;
extern std::function<uint32_t(std::string, uint32_t)> debug_sell_amount_func;



























#endif //DEBUG_API