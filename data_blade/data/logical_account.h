#ifndef LOGICAL_ACCOUNT
#define LOGICAL_ACCOUNT

#include "logical_ticker.h"


class logical_account {
    private:
        logical_ticker * _have_ticker(std::string ticker);

        std::list<logical_ticker*> _logical_tickers;
    public:
        std::string account_name();
        double available_cash();
        uint32_t buy_stock(std::string ticker);
        uint32_t sell_stock(std::string ticker);
};


























#endif