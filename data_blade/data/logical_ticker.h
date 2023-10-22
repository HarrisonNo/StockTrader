#include <inttypes.h>
#include <string>
#include <list>
#include <functional>
#include "logical_account.h"


#ifndef LOGICAL_TICKER
#define LOGICAL_TICKER


class logical_ticker {
    private:
    //Variables
        time_t _time_last_checked_price;
        time_t _time_last_checked_amount;

        uint32_t _amount_owned;

        uint8_t _known_stock_amount_owned:1;
        uint8_t _known_stock_amount_owned_locked:1;
        uint8_t _can_sell_at_loss_default:1;//Are we allowed to sell shares of this stock for a loss?
        uint8_t _UNUSED_EIGHT_BITS:5;

        double _stock_price;

        std::string _ticker;

        logical_account * _tied_account;

        time_t _time_last_checked_price;

    //Functions
        bool _double_check_transaction(uint32_t expected_held_amount, uint32_t initial_held_amount);

        void _modify_transaction_list(int32_t amount, double price = -1);//Amount to add/remove from transaction list
        void _load_transactions();
        void _save_transactions();

    //Abstract data types
        class list_insert {
            public:
                uint32_t amount;
                double price;
                inline list_insert(){};//Default, used for generating pointers
                inline list_insert(uint32_t request_amount, double request_price) {
                    amount = request_amount;
                    price = request_price;
                }
        };
        inline list_insert * _create_list_node(uint32_t amount, double price) {return new list_insert(amount, price);}
        inline void _delete_list_node(list_insert *node) {free(node);}
        //Ordered from low price -> high price
        std::list<list_insert*> _transactions;

        class historical_price_insert {
            public:
                uint64_t year:12;//Supports decimal value of 0-4065, can definitely be stored more efficiently but with substantially more effort
                uint64_t month:4;//Supports decimal value of 0-15
                uint64_t day:5;//Supprts decimal value of 0-31, cutting really close
                uint64_t hour:5;//Supports decimal value of 0-31
                uint64_t minute:6;//Supports decimal value of 0-63
                uint64_t second:6;//Supports decimal value of 0-63
                uint64_t UNUSED:26;
                double price;
        };
        
        void _save_historical_price_insert();

    public:
        logical_ticker(std::string input_ticker);

        uint32_t amount_owned();
        uint32_t purchase_amount(uint32_t amount);
        uint32_t sell_amount(uint32_t amount);

        double stock_price();

        inline void modify_selling_at_loss(bool enabled);//Allows the user to set a default behavior for whether we are allowed to sell shares at a loss, is overriden by user specification at sell time

        bool heartbeat();//Tests several calls from the wrapper to check if many operations are still possible
};



#endif//LOGICAL_TICKER