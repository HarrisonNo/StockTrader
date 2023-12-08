#include "logical_account.h"
#include "debug_func_replacements.h"
#include "debug_api.h"







bool basic_class_creation(std::string * fail_string) {
    try {
        logical_account * la;
    }
    catch (std::exception &e) {
        *fail_string = e.what();
        return false;
    }
    return true;
}



bool class_at_100() {
    debug_amount_owned_func = debug_amount_owned_return_100;//This is so ridiculously cursed
    debug_stock_price_func = debug_stock_price_increase_by_one_from_start;
}