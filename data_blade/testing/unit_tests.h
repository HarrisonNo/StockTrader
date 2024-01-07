#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H


//BASIC
//Calls buy/sell once or solely tests a single feature
bool basic_class_creation();
bool basic_heap_class_creation();
bool basic_purchase_ten();
bool basic_async_purchase_ten();
bool basic_purchase_extra();
bool basic_sell_ten();
bool basic_async_sell_ten();
bool basic_sell_extra();


#define RUN_ALL_BASIC_TESTS \
    basic_class_creation(); \
    basic_heap_class_creation(); \
    basic_purchase_ten(); \
    basic_async_purchase_ten(); \
    basic_sell_ten(); \
    basic_async_sell_ten(); \
    basic_purchase_extra(); \
    basic_sell_extra();




//INTERMEDIATE
//Calls buy/sell multiple times alongside one new feature
bool intermediate_repeated_buy_sell_one_basic();
bool intermediate_repeated_buy_sell_two_basic();
bool intermediate_sell_fully_unprofitable();
bool intermediate_sell_partially_unprofitable();
bool intermediate_force_sell_fully_unprofitable();
bool intermediate_load_saved_transactions();
bool intermediate_historical_prices_basic();
bool intermediate_repeated_buy_sell_one_advanced();


#define RUN_ALL_INTERMEDIATE_TESTS \
    intermediate_repeated_buy_sell_one_basic(); \
    intermediate_repeated_buy_sell_two_basic(); \
    intermediate_sell_fully_unprofitable(); \
    intermediate_sell_partially_unprofitable(); \
    intermediate_force_sell_fully_unprofitable(); \
    intermediate_load_saved_transactions(); \
    intermediate_historical_prices_basic(); \
    intermediate_repeated_buy_sell_one_advanced();







//ADVANCED
//Multiple buy/sells alongside multiple features, introduces occasional INTENTIONAL failures in one or more areas











//EXPERT
//Tests that start with a preset rng seed and go chaotic











#endif//UNIT_TESTS_H