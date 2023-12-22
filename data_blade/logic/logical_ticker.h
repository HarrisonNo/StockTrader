#include <inttypes.h>
#include <functional>
#include "logical_account.h"

#ifndef LOGICAL_TICKER
#define LOGICAL_TICKER

class logical_ticker {
    private:
    //Variables
        time_t _time_last_checked_price;
        time_t _time_last_checked_amount;
        time_t _time_last_executed_transaction;

        uint32_t _amount_owned;
        uint32_t _transactions_list_stock_count;

        uint8_t _known_stock_amount_owned:1;
        uint8_t _can_sell_at_loss_default:1;//Are we allowed to sell shares of this stock for a loss?
        uint8_t _UNUSED_EIGHT_BITS:6;

        double _stock_price;

        std::string _ticker;

        logical_account * _tied_account;

    //Functions
        void _modify_transaction_list(int64_t amount, double price = -1);//Amount to add/remove from transaction list
        void _load_transactions();
        void _save_transactions();
        void _save_stock_price_at_time(double stock_price, time_t current_time = 0);
        void _catch_invalid_dates(int16_t * year, int16_t * month, int16_t * day, int16_t * hour, int16_t * minute, int16_t * second);
        void _date_corrections(int16_t * min_year, int16_t * min_month, int16_t * min_day, int16_t * min_hour, int16_t * min_minute, int16_t * min_second,
                               int16_t * max_year, int16_t * max_month, int16_t * max_day, int16_t * max_hour, int16_t * max_minute, int16_t * max_second);

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

        //Ordered from low price -> high price
        std::list<list_insert*> _transactions;

    //ADT functions
        inline list_insert * _create_list_node(uint32_t amount, double price) {return new list_insert(amount, price);}
        inline void _delete_list_node(list_insert *node) {delete(node);}

    public:
        logical_ticker(std::string input_ticker, bool allow_boot_loading = true);
        ~logical_ticker();

        inline std::string ticker();

        inline void modify_selling_at_loss(bool enabled);//Allows the user to set a default behavior for whether we are allowed to sell shares at a loss, is overriden by user specification at sell time

        uint32_t amount_owned(bool force_check = false);
        uint32_t purchase_amount(uint32_t amount);
        uint32_t sell_amount(uint32_t amount, bool force_sell = false);

        void save_self();
        
        double stock_price(bool force_check = false);
        double stock_price_at_time(int16_t requested_year, int16_t requested_month, int16_t requested_day, int16_t requested_hour, int16_t requested_minute, int16_t requested_second,
                                   int16_t year_deviation = 0, int16_t month_deviation = 0, int16_t day_deviation = 0, int16_t hour_deviation = 0, int16_t minute_deviation = 0, int16_t second_deviation = 0);

        uint64_t stock_prices_between_times(uint64_t calculation_limit, double * array_bank, int16_t min_year = INT16_MIN, int16_t min_month = INT16_MIN, int16_t min_day = INT16_MIN, int16_t min_hour = INT16_MIN, int16_t min_minute = INT16_MIN, int16_t min_second = INT16_MIN,
                                                                                             int16_t max_year = INT16_MIN, int16_t max_month = INT16_MIN, int16_t max_day = INT16_MIN, int16_t max_hour = INT16_MIN, int16_t max_minute = INT16_MIN, int16_t max_second = INT16_MIN);

        bool heartbeat();//Tests several calls from the wrapper to check if many operations are still possible
};



#endif//LOGICAL_TICKER