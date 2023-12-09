#include "unit_tests.h"
#include "logical_account.h"
#include "api.h"
#include "debug_func_replacements.h"

#define UNIT_TEST_TRY_WRAPPER(X, StringVar) \
    try { \
        X \
    } \
    catch (std::exception &e) { \
        StringVar = e.what(); \
        return false; \
    }


bool basic_class_creation(std::string * fail_string) {
    UNIT_TEST_TRY_WRAPPER(
        logical_account * la;
    , *fail_string)
    return true;
}


bool basic_function_replacement(std::string * fail_string) {
    UNIT_TEST_TRY_WRAPPER(
        debug_amount_owned_func = debug_amount_owned_ZERO;
        debug_stock_price_func = debug_stock_price_ZERO;
        debug_account_cash_func = debug_account_cash_ZERO;
    , *fail_string)
    return true;
}


bool basic_function_replacement(std::string * fail_string) {
    UNIT_TEST_TRY_WRAPPER(
        //MAKE ABOUT GLOBAL VAL SET
        debug_account_cash_GLOBAL_var = 100;
        debug_amount_owned_func = debug_amount_owned_GLOBAL;
        debug_stock_price_func = debug_stock_price_ZERO;
        debug_account_cash_func = debug_account_cash_ZERO;
    , *fail_string)
    return true;
}