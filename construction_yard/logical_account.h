#include <string>
#include <mutex>
#include <list>
#include <map>
#include "api.h"

#ifndef LOGICAL_ACCOUNT
#define LOGICAL_ACCOUNT

//Forward declaration of ticker
class logical_ticker;

typedef long long key;

class logical_account {
    private:
    //Variables
        std::string _account_name;

        double _cash;
        double _projected_cash;//Accounts for async threads that may take a minute to execute

        uint8_t _number_of_projections;

        uint64_t _cheap_rng;

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

        inline key _generate_key(std::string ticker, uint32_t amount = UINT32_MAX);

        inline void _save_self();

        void _async_buy_stock_wrapper(std::string ticker, uint32_t amount, key async_key);
        void _async_sell_stock_wrapper(std::string ticker, uint32_t amount, key async_key);
        void _async_stock_price_wrapper(std::string ticker, bool force_check);

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
        logical_account(std::string account_name = "PLACEHOLDER", bool load_existing = true);
        
        inline std::string account_name();

        inline void mark_known_cash_unkown() {_known_cash_amount = 0;}

        inline uint32_t get_key_value(key requested_key, bool auto_delete_entry = true);
        inline uint32_t wait_for_key_value(key requested_key, bool auto_delete_entry = true);

        inline bool key_has_returned_value(key requested_key);

        double available_cash(bool force_check = false);
        double stock_price(std::string ticker, bool force_check = false);

        uint32_t buy_stock(std::string ticker, uint32_t amount);
        uint32_t sell_stock(std::string ticker, uint32_t amount, bool force_sell = false);
        uint32_t held_stock(std::string ticker, bool force_check = false);

        key async_buy_stock(std::string ticker, uint32_t amount, bool generate_key = true);//Generate and return a key then dispatch thread, key is used to access list(?) which holds the desired returned value
        key async_sell_stock(std::string ticker, uint32_t amount, bool generate_key = true);

        void async_stock_price(std::string ticker, bool force_check = false);//Keys are currently not generated for stock_price

        wrapper_class * get_wrapper_class() {return &_wrapper_vars;}

        void trigger_mass_save();//Called to save everything and prepare for shutdown
};
























#endif