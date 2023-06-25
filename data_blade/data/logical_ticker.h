#ifndef LOGICAL_TICKER
#define LOGICAL_TICKER

#include <inttypes.h>
#include <string>
#include <list>
#include <functional>
#include "logical_account.h"

typedef uint32_t (logical_ticker::*uint32_t_func)();//Allows for variables inside logical_ticker to act as generic functions that return uint32_t

class logical_ticker {
    private:
        time_t _time_last_checked_price;

        uint32_t _amount_owned;

        uint32_t_func _amount_owned_func;//Can be set to either _amount_owed_mine_func or _amount_owned_wrapper_func
        
        inline uint32_t _amount_owned_mine_func();
        uint32_t _amount_owned_wrapper_func();

        bool _double_check_transaction(uint32_t expected_held_amount, uint32_t attempted_to_trade_amount, uint32_t initial_held_amount);

        double _stock_price;

        std::string _ticker;

        logical_account * _tied_account;

        struct list_insert {
            uint32_t amount;
            double price;
            inline list_insert(){};//Default, used for generating pointers
            inline list_insert(uint32_t request_amount, double request_price) {
                amount = request_amount; 
                price = request_price;
            }
        };
        std::list<list_insert*> _transactions;

        time_t _time_last_checked_price;
    public:
        logical_ticker(std::string input_ticker);

        uint32_t amount_owned();
        uint32_t purchase_amount(uint32_t amount);
        uint32_t sell_amount(uint32_t amount);

        double stock_price();

        bool heartbeat();//Tests several calls from the wrapper to check if many operations are still possible
};



#endif//LOGICAL_TICKER