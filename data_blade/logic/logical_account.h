#include <string>
#include <mutex>
#include <list>
#include <map>
#include "api.h"

#ifndef LOGICAL_ACCOUNT
#define LOGICAL_ACCOUNT

#include "logical_ticker.h"//Acts as forward declaration of logical_ticker

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
        inline logical_ticker * _get_logical_ticker(std::string ticker);//SWAP TO MAP OF LOGICAL_TICKERS
        inline logical_ticker * _create_logical_ticker(std::string ticker);
        inline logical_ticker * _get_or_create_logical_ticker(std::string ticker);

        inline key _generate_key(std::string ticker, uint32_t amount);

        void _async_buy_stock_wrapper(std::string ticker, uint32_t amount, key async_key);
        void _async_sell_stock_wrapper(std::string ticker, uint32_t amount, key async_key);

    //Abstract data types
        class async_return {
            public:
                key stored_key;
                bool has_return_value;
                uint32_t return_value;
                inline async_return(){}//Default, used for generating pointers
                inline async_return(key sk) {
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

        std::map<key, async_return*> _keyed_transactions;
        std::map<std::string, logical_ticker*> _logical_tickers;
    public:
        logical_account(bool load_existing = true);
        
        inline std::string account_name();

        inline void mark_known_cash_unkown() {_known_cash_amount = 0;}

        inline uint32_t get_key_value(key requested_key, bool auto_delete_entry = true);
        inline uint32_t wait_for_key_value(key requested_key, bool auto_delete_entry = true);

        inline bool key_has_returned_value(key requested_key);

        double available_cash(bool force_check = false);

        uint32_t buy_stock(std::string ticker, uint32_t amount);
        uint32_t sell_stock(std::string ticker, uint32_t amount, bool force_sell = false);
        uint32_t held_stock(std::string ticker, bool force_check = false);

        key async_buy_stock(std::string ticker, uint32_t amount);//Generate and return a key then dispatch thread, key is used to access list(?) which holds the desired returned value
        key async_sell_stock(std::string ticker, uint32_t amount);

        wrapper_class * get_wrapper_class() {return &_wrapper_vars;}

        void trigger_clean_shutdown();//Called to save everything and prepare for shutdown
};
























#endif