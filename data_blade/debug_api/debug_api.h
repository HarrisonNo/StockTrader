#ifndef DEBUG_API_FILE
#define DEBUG_API_FILE

#include <string>



//uint32_t debug_amount_owned(std::string ticker);

extern std::function<uint32_t(std::string)> debug_amount_owned_func;



inline uint32_t debug_amount_owned(std::string ticker);
double debug_stock_price(std::string ticker);











































#endif //DEBUG_API