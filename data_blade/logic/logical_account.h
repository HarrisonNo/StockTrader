#include <string>
#include <mutex>
#include <list>
#include <map>
#include <future>
#include "api.h"

#ifndef LOGICAL_ACCOUNT
#define LOGICAL_ACCOUNT

//Forward declaration of ticker
class logical_ticker;

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

        inline void _save_self();

    //Abstract data types
        class transaction_checker_insert {//https://en.cppreference.com/w/cpp/thread/async <----------------------------USE INSTEAD LMAO
            public:
                logical_ticker * lt;
                uint32_t expected_amount;
        };

        std::map<std::string, logical_ticker*> _logical_tickers;
    public:
        logical_account(std::string account_name = "PLACEHOLDER", bool load_existing = true);
        ~logical_account();

        inline void mark_known_cash_unkown() {_known_cash_amount = 0;}

        double available_cash(bool force_check = false);
        double stock_price(std::string ticker, bool force_check = false);

        uint32_t buy_stock(std::string ticker, uint32_t amount);
        uint32_t sell_stock(std::string ticker, uint32_t amount, bool force_sell = false);
        uint32_t held_stock(std::string ticker, bool force_check = false);

        std::future<uint32_t> async_buy_stock(std::string ticker, uint32_t amount);
        std::future<uint32_t> async_sell_stock(std::string ticker, uint32_t amount, bool force_sell = false);
        std::future<double> async_stock_price(std::string ticker, bool force_check = false);

        void trigger_mass_save();//Called to save everything and prepare for shutdown

        wrapper_class * get_wrapper_class() {return &_wrapper_vars;}

        std::string account_name();
};
























#endif