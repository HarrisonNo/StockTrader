#include "debug_func_replacements.h"


uint32_t debug_amount_owned_return_100(std::string ticker) {
    return 100;
}


double debug_stock_price_increase_by_one_from_start(std::string ticker) {
    static double starting_price = 500;
    return ++starting_price;
}