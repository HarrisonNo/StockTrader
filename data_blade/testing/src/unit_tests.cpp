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
#define THROW_IF_FALSE_TWO(Statement, Var, VarTwo) if (!(Statement)) {throw "Failed unit test line "+std::to_string(__LINE__)+" which is '"#Statement"' with values: "+std::to_string(Var)+" and "+std::to_string(VarTwo);}
#define THROW_IF_FALSE_THREE(Statement, Var, VarTwo, VarThree) if (!(Statement)) {throw "Failed unit test line "+std::to_string(__LINE__)+" which is '"#Statement"' with values: "+std::to_string(Var)+", "+std::to_string(VarTwo)+" and "+std::to_string(VarThree);}
#define THROW_IF_FALSE_FOUR(Statement, Var, VarTwo, VarThree, VarFour) if (!(Statement)) {throw "Failed unit test line "+std::to_string(__LINE__)+" which is '"#Statement"' with values: "+std::to_string(Var)+", "+std::to_string(VarTwo)+", "+std::to_string(VarThree)+" and "+std::to_string(VarFour);}


//BASIC


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

bool basic_purchase_extra() {
    UNIT_TEST_TRY_WRAPPER(
        //Set globals and defaults
        debug_account_cash_SET_GLOBAL(110);
        debug_stock_price_SET_GLOBAL("MSFT", 25);
        //Start program
        logical_account la("test", false);
        uint32_t bought_stock = la.held_stock("MSFT");
        double account_cash = la.available_cash();
        THROW_IF_FALSE(bought_stock == 0, bought_stock);
        THROW_IF_FALSE(account_cash == 110, account_cash);
        la.buy_stock("MSFT", 10);//Attempts to buy 10 when we only have the money for 4 shares
        bought_stock = la.held_stock("MSFT");
        account_cash = la.available_cash();
        THROW_IF_FALSE(bought_stock == 4, bought_stock);
        THROW_IF_FALSE(account_cash == 10, account_cash);
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

bool basic_sell_extra() {
    UNIT_TEST_TRY_WRAPPER(
        //Set globals and defaults
        debug_account_cash_SET_GLOBAL(0);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        debug_amount_owned_SET_GLOBAL("MSFT", 4);
        //Start program
        logical_account la("test", false);
        uint32_t sold_stock = la.held_stock("MSFT");
        double account_cash = la.available_cash();
        THROW_IF_FALSE(sold_stock == 4, sold_stock);
        THROW_IF_FALSE(account_cash == 0, account_cash);
        la.sell_stock("MSFT", 10);//Attempt to sell 10 even though we only have 4 shares
        sold_stock = la.held_stock("MSFT");
        account_cash = la.available_cash();
        THROW_IF_FALSE(sold_stock == 0, sold_stock);
        THROW_IF_FALSE(account_cash == 40, account_cash);
    )
}


//INTERMEDIATE


//Both TEMPLATED_BUY/SELL_STOCK assume that any and all trades complete 100% successfully
//Separate defines must be created for INTENTIONAL potentially incomplete trades
inline void TEMPLATED_UNSAFE_BUY_STOCK(std::string ticker, logical_account * account, uint32_t to_buy_amount) {
    uint32_t intial_held_stock = account->held_stock(ticker);
    double intial_cash_amount = account->available_cash();
    double initial_stock_price = account->stock_price(ticker, true);//Have to force it to call the api since we change the price so rapidly lmao
    account->buy_stock(ticker, to_buy_amount);
    uint32_t final_stock_amount = account->held_stock(ticker);
    double final_cash_amount = account->available_cash();
    THROW_IF_FALSE_THREE(final_stock_amount == (intial_held_stock + to_buy_amount), final_stock_amount, intial_held_stock, to_buy_amount);
    THROW_IF_FALSE_FOUR(final_cash_amount == (intial_cash_amount - (initial_stock_price * to_buy_amount)), final_cash_amount, intial_cash_amount, initial_stock_price, to_buy_amount);
}

inline void TEMPLATED_UNSAFE_SELL_STOCK(std::string ticker, logical_account * account, uint32_t to_sell_amount, bool force_sell = false) {
    uint32_t intial_held_stock = account->held_stock(ticker);
    double intial_cash_amount = account->available_cash();
    double initial_stock_price = account->stock_price(ticker, true);//Have to force it to call the api since we change the price so rapidly lmao
    account->sell_stock(ticker, to_sell_amount, force_sell);
    uint32_t final_stock_amount = account->held_stock(ticker);
    double final_cash_amount = account->available_cash();
    THROW_IF_FALSE_THREE(final_stock_amount == (intial_held_stock - to_sell_amount), final_stock_amount, intial_held_stock, to_sell_amount);
    THROW_IF_FALSE_FOUR(final_cash_amount == (intial_cash_amount + (initial_stock_price * to_sell_amount)), final_cash_amount, intial_cash_amount, initial_stock_price, to_sell_amount);
}


bool intermediate_buy_sell_one_stock() {
    UNIT_TEST_TRY_WRAPPER(
        debug_account_cash_SET_GLOBAL(100);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        //amount owned for msft should auto-default to 0
        logical_account la("test", false);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 10);//$0 remaining
        debug_stock_price_SET_GLOBAL("MSFT", 20);
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", &la, 10);//$200 remaining
        debug_stock_price_SET_GLOBAL("MSFT", 40);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 5);//$0 remaining
        debug_stock_price_SET_GLOBAL("MSFT", 50);
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", &la, 5);//$250 remaining
    )
}

bool intermediate_buy_sell_two_stock() {
    UNIT_TEST_TRY_WRAPPER(
        debug_account_cash_SET_GLOBAL(200);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        debug_stock_price_SET_GLOBAL("AAPL", 20);
        //amount owned for msft should auto-default to 0
        logical_account la("test", false);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 10);//$100 remaining
        debug_stock_price_SET_GLOBAL("MSFT", 20);
        TEMPLATED_UNSAFE_BUY_STOCK("AAPL", &la, 3);//$40 remaining
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", &la, 10);//$240 remaining
        debug_stock_price_SET_GLOBAL("MSFT", 40);
        debug_stock_price_SET_GLOBAL("AAPL", 25);
        TEMPLATED_UNSAFE_SELL_STOCK("AAPL", &la, 3);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 5);
        debug_stock_price_SET_GLOBAL("MSFT", 50);
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", &la, 5);
    )
}

bool intermediate_sell_fully_unprofitable() {
    UNIT_TEST_TRY_WRAPPER(
        debug_account_cash_SET_GLOBAL(100);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        logical_account la("test", false);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 10);
        debug_stock_price_SET_GLOBAL("MSFT", 5);
        la.stock_price("MSFT", true);
        la.sell_stock("MSFT", 10);//Should fully fail
        double account_cash = la.available_cash();
        uint32_t stock_held = la.held_stock("MSFT");
        THROW_IF_FALSE(account_cash == 0, account_cash);
        THROW_IF_FALSE(stock_held == 10, stock_held);
        debug_stock_price_SET_GLOBAL("MSFT", 20);
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", &la, 10);//Should fully succeed
    )
}

bool intermediate_sell_partially_unprofitable() {
    UNIT_TEST_TRY_WRAPPER(
        debug_account_cash_SET_GLOBAL(200);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        logical_account la("test", false);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 10);//$100 remaining
        debug_stock_price_SET_GLOBAL("MSFT", 20);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 5);//$0 remaining
        debug_stock_price_SET_GLOBAL("MSFT", 15);
        la.sell_stock("MSFT", 20);//Should partially fail, 10 shares we bought at $10 and 5 shares we bought at $20 and 5 shares we don't actually have
        double account_cash = la.available_cash();
        uint32_t stock_held = la.held_stock("MSFT");
        THROW_IF_FALSE(account_cash == 150, account_cash);
        THROW_IF_FALSE(stock_held == 5, stock_held);
        debug_stock_price_SET_GLOBAL("MSFT", 25);
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", &la, 5);//Should fully succeed
    )
}

bool intermediate_force_sell_fully_unprofitable() {
    UNIT_TEST_TRY_WRAPPER(
        debug_account_cash_SET_GLOBAL(100);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        logical_account la("test", false);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", &la, 10);
        debug_stock_price_SET_GLOBAL("MSFT", 5);
        la.sell_stock("MSFT", 10);//Should fully fail
        double account_cash = la.available_cash();
        uint32_t stock_held = la.held_stock("MSFT");
        THROW_IF_FALSE(account_cash == 0, account_cash);
        THROW_IF_FALSE(stock_held == 10, stock_held);
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", &la, 10, true);//Should fully succeed
    )
}

bool intermediate_load_saved_transactions() {
    UNIT_TEST_TRY_WRAPPER(
        debug_account_cash_SET_GLOBAL(200);
        debug_stock_price_SET_GLOBAL("MSFT", 10);
        logical_account * og_la = new logical_account("test", false);
        TEMPLATED_UNSAFE_BUY_STOCK("MSFT", og_la, 10);
        delete(og_la);
        debug_stock_price_SET_GLOBAL("MSFT", 5);
        logical_account * newer_la = new logical_account("test");//Let account load
        newer_la->sell_stock("MSFT", 10);//Should fully fail
        double account_cash = newer_la->available_cash();
        uint32_t stock_held = newer_la->held_stock("MSFT");
        THROW_IF_FALSE(account_cash == 100, account_cash);
        THROW_IF_FALSE(stock_held == 10, stock_held);
        debug_stock_price_SET_GLOBAL("MSFT", 20);
        TEMPLATED_UNSAFE_SELL_STOCK("MSFT", newer_la, 10, true);//Should fully succeed
        delete(newer_la);
    )
}

//ADD UNIT TESTS FOR TICKER PRICE HISTORY