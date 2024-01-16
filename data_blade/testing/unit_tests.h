#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H


//BASIC
//Calls buy/sell once or solely tests a single feature
bool basic_class_creation(bool silent = false);
bool basic_heap_class_creation(bool silent = false);
bool basic_purchase_ten(bool silent = false);
bool basic_async_purchase_ten(bool silent = false);
bool basic_purchase_extra(bool silent = false);
bool basic_sell_ten(bool silent = false);
bool basic_async_sell_ten(bool silent = false);
bool basic_sell_extra(bool silent = false);


#define RUN_FUNC_ON_BASIC_TESTS(Global_Func, Func, Testing_Name) \
    Global_Func( \
    Func(basic_class_creation) \
    Func(basic_heap_class_creation) \
    Func(basic_purchase_ten) \
    Func(basic_async_purchase_ten) \
    Func(basic_sell_ten) \
    Func(basic_async_sell_ten) \
    Func(basic_purchase_extra) \
    Func(basic_sell_extra) \
    , \
    Testing_Name \
    )


//INTERMEDIATE
//Calls buy/sell multiple times alongside one new feature
bool intermediate_repeated_buy_sell_one_basic(bool silent = false);
bool intermediate_repeated_buy_sell_two_basic(bool silent = false);
bool intermediate_sell_fully_unprofitable(bool silent = false);
bool intermediate_sell_partially_unprofitable(bool silent = false);
bool intermediate_force_sell_fully_unprofitable(bool silent = false);
bool intermediate_load_saved_transactions(bool silent = false);
bool intermediate_historical_prices_basic(bool silent = false);
bool intermediate_repeated_buy_sell_one_advanced(bool silent = false);
bool intermediate_repeated_buy_sell_one_expert(bool silent = false);


#define RUN_FUNC_ON_INTERMEDIATE_TESTS(Global_Func, Func, Testing_Name) \
    Global_Func( \
    Func(intermediate_repeated_buy_sell_one_basic) \
    Func(intermediate_repeated_buy_sell_two_basic) \
    Func(intermediate_sell_fully_unprofitable) \
    Func(intermediate_sell_partially_unprofitable) \
    Func(intermediate_force_sell_fully_unprofitable) \
    Func(intermediate_load_saved_transactions) \
    Func(intermediate_historical_prices_basic) \
    Func(intermediate_repeated_buy_sell_one_advanced) \
    Func(intermediate_repeated_buy_sell_one_expert) \
    , \
    Testing_Name \
    )



//ADVANCED
bool advanced_account_functionality_basic(bool silent = false);



#define RUN_FUNC_ON_ADVANCED_TESTS(Global_Func, Func, Testing_Name) \
    Global_Func( \
    Func(advanced_account_functionality_basic) \
    , \
    Testing_Name \
    )



#define RUN_SILENT_GLOBAL_FUNC(tests, tests_name) \
    do { \
        bool succeeded = true; \
        bool silent_mode = true; \
        tests \
        if (!succeeded) { \
            std::cout<<tests_name<<" unit test FAILED, iterating through individually"<<std::endl<<std::endl; \
            silent_mode = false; \
            tests \
        } \
        if (succeeded) { \
            std::cout<<tests_name<<" unit tests PASSED"<<std::endl<<std::endl; \
        } \
    } while(false);

#define RUN_NORMAL_GLOBAL_FUNC(tests, tests_name) \
    tests

#define RUN_SILENT_FUNC(unit_test) \
    if ((!silent_mode || succeeded) && !unit_test(silent_mode)) { \
        succeeded = false; \
    }

#define RUN_NORMAL_FUNC(unit_test) \
    unit_test();


#define RUN_BASIC                   RUN_FUNC_ON_BASIC_TESTS(RUN_NORMAL_GLOBAL_FUNC, RUN_NORMAL_FUNC, "BASIC")
#define RUN_BASIC_SILENT            RUN_FUNC_ON_BASIC_TESTS(RUN_SILENT_GLOBAL_FUNC, RUN_SILENT_FUNC, "BASIC")

#define RUN_INTERMEDIATE            RUN_FUNC_ON_INTERMEDIATE_TESTS(RUN_NORMAL_GLOBAL_FUNC, RUN_NORMAL_FUNC, "INTERMEDIATE")
#define RUN_INTERMEDIATE_SILENT     RUN_FUNC_ON_INTERMEDIATE_TESTS(RUN_SILENT_GLOBAL_FUNC, RUN_SILENT_FUNC, "INTERMEDIATE")

#define RUN_ADVANCED                RUN_FUNC_ON_ADVANCED_TESTS(RUN_NORMAL_GLOBAL_FUNC, RUN_NORMAL_FUNC, "ADVANCED")





//ADVANCED
//Multiple buy/sells alongside multiple features, introduces occasional INTENTIONAL failures in one or more areas











//EXPERT
//Tests that start with a preset rng seed and go chaotic











#endif//UNIT_TESTS_H