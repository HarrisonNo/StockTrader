#include <iostream>
#include "unit_tests.h"
#include "logical_account.h"
#include "api.h"
#include "debug_func_replacements.h"

#define UNIT_TEST_TRY_WRAPPER(X) \
    std::string FailString; \
    bool ReturnVal = true; \
    RESET_ALL_GLOBAL_VALUES \
    std::cout<<"Running unit test "<<__func__<<std::endl; \
    try { \
        X \
    } \
    catch (std::string st) { \
        FailString = st; \
        ReturnVal = false; \
    } \
    catch (std::exception &e) { \
        FailString = e.what(); \
        ReturnVal = false; \
    } \
    if (ReturnVal) { \
        std::cout<<"Status: PASSED"<<std::endl<<std::endl; \
    } else { \
        std::cout<<"Status: FAILED with string: "<<FailString<<std::endl<<std::endl; \
    } \
    return ReturnVal;


#define THROW_IF_FALSE(Statement, Var) if (!(Statement)) {throw "Failed unit test line "+std::to_string(__LINE__)+" which is '"#Statement"' with a value of "+std::to_string(Var);}//TODO have ASSERTS mimic this?

bool basic_class_creation() {
    UNIT_TEST_TRY_WRAPPER
    (
        logical_account la("test", false);
    )
}


//This is somewhat redundant, we aren't expecting anything different by calling new
//The main point of this is to run after basic_class_creation to showcase that we can create and destroy the account twice in a row with no saving/loading issues
bool basic_heap_class_creation() {
    UNIT_TEST_TRY_WRAPPER(
        logical_account * la = new logical_account("test", false);
        delete(la);
    )
}


bool basic_purchase_ten() {
    UNIT_TEST_TRY_WRAPPER(
        //Set globals and defaults
        debug_account_cash_SET_GLOBAL(100);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        //Start program
        logical_account la("test", false);
        uint32_t bought_stock = la.held_stock("MSFT");
        double account_cash = la.available_cash();
        THROW_IF_FALSE(bought_stock == 0, bought_stock);
        THROW_IF_FALSE(account_cash == 100, account_cash);
        la.buy_stock("MSFT", 10);
        bought_stock = la.held_stock("MSFT");
        account_cash = la.available_cash();
        THROW_IF_FALSE(bought_stock == 10, bought_stock);
        THROW_IF_FALSE(account_cash == 0, account_cash);
    )
}


bool basic_async_purchase_ten() {
    UNIT_TEST_TRY_WRAPPER(
        //Set globals and defaults
        debug_account_cash_SET_GLOBAL(100);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        //Start program
        logical_account la("test", false);
        uint32_t bought_stock = la.held_stock("MSFT");
        double account_cash = la.available_cash();
        THROW_IF_FALSE(bought_stock == 0, bought_stock);
        THROW_IF_FALSE(account_cash == 100, account_cash);
        auto key = la.async_buy_stock("MSFT", 10);
        key.wait();
        bought_stock = la.held_stock("MSFT");
        account_cash = la.available_cash();
        THROW_IF_FALSE(bought_stock == 10, bought_stock);
        THROW_IF_FALSE(account_cash == 0, account_cash);
    )
}


bool basic_sell_ten() {
    UNIT_TEST_TRY_WRAPPER(
        //Set globals and defaults
        debug_account_cash_SET_GLOBAL(0);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        debug_amount_owned_SET_GLOBAL("MSFT", 10);
        //Start program
        logical_account la("test", false);
        uint32_t sold_stock = la.held_stock("MSFT");
        double account_cash = la.available_cash();
        THROW_IF_FALSE(sold_stock == 10, sold_stock);
        THROW_IF_FALSE(account_cash == 0, account_cash);
        la.sell_stock("MSFT", 10);
        sold_stock = la.held_stock("MSFT");
        account_cash = la.available_cash();
        THROW_IF_FALSE(sold_stock == 0, sold_stock);
        THROW_IF_FALSE(account_cash == 100, account_cash);
    )
}


bool basic_async_sell_ten() {
    UNIT_TEST_TRY_WRAPPER(
        //Set globals and defaults
        debug_account_cash_SET_GLOBAL(0);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        debug_amount_owned_SET_GLOBAL("MSFT", 10);
        //Start program
        logical_account la("test", false);
        uint32_t sold_stock = la.held_stock("MSFT");
        double account_cash = la.available_cash();
        THROW_IF_FALSE(sold_stock == 10, sold_stock);
        THROW_IF_FALSE(account_cash == 0, account_cash);
        auto key = la.async_sell_stock("MSFT", 10);
        key.wait();
        sold_stock = la.held_stock("MSFT");
        account_cash = la.available_cash();
        THROW_IF_FALSE(sold_stock == 0, sold_stock);
        THROW_IF_FALSE(account_cash == 100, account_cash);
    )
}