#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H


//BASIC
//Calls buy/sell once or solely tests a single feature
bool basic_class_creation();
bool basic_heap_class_creation();
bool basic_purchase_ten();
bool basic_async_purchase_ten();
bool basic_purchas_extra();
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
    basic_purchas_extra(); \
    basic_sell_extra();




//INTERMEDIATE
//Calls buy/sell multiple times alongside one new feature
bool intermediate_buy_sell_one_stock();
bool intermediate_buy_sell_two_stock();




#define RUN_ALL_INTERMEDIATE_TESTS \
    intermediate_buy_sell_one_stock(); \
    intermediate_buy_sell_two_stock();







//ADVANCED
//Multiple buy/sells alongside multiple features, introduces occasional INTENTIONAL failures in one or more areas











//EXPERT
//Tests that start with a preset rng seed and go chaotic











#endif//UNIT_TESTS_H