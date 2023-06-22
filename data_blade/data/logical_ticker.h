#include <inttypes.h>
#include <string>
#include <list>
#include "logical_account.h"

#ifndef LOGICAL_TICKER
#define LOGICAL_TICKER

class logical_ticker {
    private:
        uint32_t _amount_owned;

        double _stock_price;

        std::string ticker;

        logical_account * tied_account;

        std::list<list_insert*> transactions;
    public:
        uint32_t amount_owned();
        uint32_t purchase_amount(uint32_t amount);
        uint32_t sell_amount(uint32_t amount);

        double stock_price();

        bool heartbeat();//Tests several calls from the wrapper to check if many operations are still possible
};

struct list_insert {
    uint32_t amount;
    
    double price;
    
    inline list_insert();//Default, used for generating pointers
    inline list_insert(uint32_t request_amount, double request_price) {
        amount = request_amount; 
        price = request_price;
    }
};

#endif//LOGICAL_TICKER