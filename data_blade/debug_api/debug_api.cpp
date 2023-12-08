#include <functional>
#include "debug_api.h"



std::function<uint32_t(std::string)> debug_amount_owned_func;




inline uint32_t debug_amount_owned(std::string ticker){
    return debug_amount_owned_func(ticker);
}
double debug_stock_price(std::string ticker);



