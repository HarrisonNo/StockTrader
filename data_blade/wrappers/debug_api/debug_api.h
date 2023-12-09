#include <string>
#include <functional>

#ifndef DEBUG_API_FILE
#define DEBUG_API_FILE




extern std::function<uint32_t(std::string)> debug_amount_owned_func;
extern std::function<double(std::string)> debug_stock_price_func;
extern std::function<double()> debug_account_cash_func;


inline uint32_t debug_amount_owned(std::string ticker);
double debug_stock_price(std::string ticker);
double debug_account_cash();











































#endif //DEBUG_API