#ifndef LOGICAL_ACCOUNT
#define LOGICAL_ACCOUNT

#include "logical_ticker.h"
#include "wrapper_class.h"
#include <mutex>

typedef long long key;


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

        wrapper_class _wrapper_vars;

    //Functions
        inline logical_ticker * _get_logical_ticker(std::string ticker);
        inline logical_ticker * _create_logical_ticker(std::string ticker);
        inline logical_ticker * _get_or_create_logical_ticker(std::string ticker);

        inline key _generate_key(std::string ticker, uint32_t amount);

        void _async_buy_stock_wrapper(std::string ticker, uint32_t amount, key async_key);
        void _async_sell_stock_wrapper(std::string ticker, uint32_t amount, key async_key);

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

        inline keyed_list_insert * _create_keyed_list_node(key sk) {return new keyed_list_insert(sk);}
        inline keyed_list_insert * _get_kli_from_list(key sk) {for (std::list<keyed_list_insert*>::iterator it = _keyed_transactions.begin(); it != _keyed_transactions.end(); ++it) {if ((*it)->stored_key == sk) {return *it;}}}

        std::list<keyed_list_insert*> _keyed_transactions;
        std::list<logical_ticker*> _logical_tickers;
    public:
        inline std::string account_name();

        inline void mark_known_cash_unkown() {_known_cash_amount = 0;}

        double available_cash(bool force_wrapper_check = false);

        uint32_t buy_stock(std::string ticker, uint32_t amount);
        uint32_t sell_stock(std::string ticker, uint32_t amount);

        inline uint32_t get_key_value(key requested_key, bool auto_delete_entry = true);

        inline bool key_has_returned_value(key requested_key) {keyed_list_insert * kli = _get_kli_from_list(requested_key); return kli ? kli->has_return_value : false;}

        key async_buy_stock(std::string ticker, uint32_t amount);//Generate and return a key then dispatch thread, key is used to access list(?) which holds the desired returned value
        key async_sell_stock(std::string ticker, uint32_t amount);

        wrapper_class * get_wrapper_class() {return &_wrapper_vars;}
};
























#endif