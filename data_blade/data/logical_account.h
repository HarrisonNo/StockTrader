#ifndef LOGICAL_ACCOUNT
#define LOGICAL_ACCOUNT



class logical_account {
    private:
    public:
        std::string account_name();
        double available_cash();
        uint32_t buy_stock(std::string Ticker);
        uint32_t sell_stock(std::string Ticker);
};


























#endif