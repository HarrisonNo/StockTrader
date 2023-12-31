#include <inttypes.h>
#include <functional>
#include <list>
#include "logical_account.h"

#ifndef LOGICAL_TICKER
#define LOGICAL_TICKER

class logical_ticker {
    private:
    //Variables
        time_t _time_last_checked_price;
        time_t _time_last_checked_amount;
        time_t _time_last_executed_transaction;

        int _loaded_year;
        int _loaded_month;

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

        //Ordered from low price -> high price
        std::list<list_insert*> _transactions;
        //Stores historical prices
        std::vector<std::pair<time_t, double>> _historical_prices_month_file;
        //Stores historical prices within specified range
        std::vector<std::pair<time_t, double>> _historical_prices_ranged;

    //ADT functions
        inline list_insert * _create_list_node(uint32_t amount, double price) {return new list_insert(amount, price);}
        inline void _delete_list_node(list_insert *node) {delete(node);}

        void _load_historical_price_file(int month = INT_MAX, int year = INT_MAX);//Corrects monthly held prices

    public:
        logical_ticker(std::string input_ticker, logical_account * tied_account, bool allow_boot_loading = true);
        ~logical_ticker();

        inline std::string ticker();

        uint32_t amount_owned(bool force_check = false);
        uint32_t purchase_amount(uint32_t amount);
        uint32_t sell_amount(uint32_t amount, bool force_sell = false);

        void save_self();
        void modify_selling_at_loss(bool enabled);//Allows the user to set a default behavior for whether we are allowed to sell shares at a loss, is overriden by user specification at sell time
        
        double stock_price(bool force_check = false);

        std::vector<std::pair<time_t, double>> * load_historical_prices(time_t start_time, time_t end_time);//Returns pointer to loaded vector

        bool heartbeat();//Tests several calls from the wrapper to check if many operations are still possible
};



#endif//LOGICAL_TICKER