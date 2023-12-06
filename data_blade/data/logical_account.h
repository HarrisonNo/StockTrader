#ifndef LOGICAL_ACCOUNT
#define LOGICAL_ACCOUNT

#include "logical_ticker.h"
#include "wrapper_class.h"
#include <mutex>

typedef long long key;

#define MAX_STOCK_PRICE_TIMEOUT 5 //Number of seconds we can use our stored stock price for a ticker before needing to check it again
#define MAX_KNOWN_SEC_TIMEOUT 21600 //Max number of seconds we can use our stored known amount before double checking (6 hours)
#define MIN_KNOWN_SEC_TIMEOUT 60 //Min number of seconds we have to double check before we can use our stored known amount

class logical_account {
    private:
    //Variables
        std::string _account_name;

        double _cash;
        double _projected_cash;//Accounts for async threads that may take a minute to execute

        uint8_t _number_of_projections;

        uint8_t _known_cash_amount:1;
        uint8_t _UNUSED_EIGHT_BITS:7;

        std::mutex _cash_lock;
        std::mutex _transaction_check_lock;

        time_t _time_last_checked_cash;

        wrapper_class _wrapper_vars;

    //Functions
        inline logical_ticker * _get_logical_ticker(std::string ticker);
        inline logical_ticker * _create_logical_ticker(std::string ticker);
        inline logical_ticker * _get_or_create_logical_ticker(std::string ticker);

        inline key _generate_key(std::string ticker, uint32_t amount);

        void _async_buy_stock_wrapper(std::string ticker, uint32_t amount, key async_key);
        void _async_sell_stock_wrapper(std::string ticker, uint32_t amount, key async_key);
        void _solo_thread_transaction_checker();

    //Abstract data types
        class keyed_list_insert {
            public:
                key stored_key;
                bool has_return_value;
                uint32_t return_value;
                inline keyed_list_insert(){}//Default, used for generating pointers
                inline keyed_list_insert(key sk) {
                    stored_key = sk;
                    has_return_value = false;
                    return_value = 0;
                }
        };

        class transaction_checker_insert {
            public:
                logical_ticker * lt;
                uint32_t expected_amount;
        };

        std::list<keyed_list_insert*> _keyed_transactions;
        std::list<logical_ticker*> _logical_tickers;

    //ADT functions
        inline keyed_list_insert * _get_kli_from_list(key sk) {for (std::list<keyed_list_insert*>::iterator it = _keyed_transactions.begin(); it != _keyed_transactions.end(); ++it) {if ((*it)->stored_key == sk) {return *it;}}}
    public:
        logical_account();
        
        inline std::string account_name();

        inline void mark_known_cash_unkown() {_known_cash_amount = 0;}

        inline uint32_t get_key_value(key requested_key, bool auto_delete_entry = true);
        inline uint32_t wait_for_key_value(key requested_key, bool auto_delete_entry = true);

        inline bool key_has_returned_value(key requested_key) {keyed_list_insert * kli = _get_kli_from_list(requested_key); return kli ? kli->has_return_value : false;}

        double available_cash(bool force_check = false);

        uint32_t buy_stock(std::string ticker, uint32_t amount);
        uint32_t sell_stock(std::string ticker, uint32_t amount);

        key async_buy_stock(std::string ticker, uint32_t amount);//Generate and return a key then dispatch thread, key is used to access list(?) which holds the desired returned value
        key async_sell_stock(std::string ticker, uint32_t amount);

        wrapper_class * get_wrapper_class() {return &_wrapper_vars;}

        void trigger_clean_shutdown();//Called to save everything and prepare for shutdown
};
























#endif