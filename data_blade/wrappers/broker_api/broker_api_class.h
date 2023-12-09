#ifndef WRAPPER_CLASS
#define WRAPPER_CLASS

#include <string>
#include <inttypes.h>

class wrapper_class {
    public:
    //Variables
        int test_wrapper_value;

        wrapper_class(){
            test_wrapper_value = 0;
        }

    //Functions
        double wrapper_stock_price(std::string ticker);
        double wrapper_account_cash();

        uint32_t wrapper_amount_owned(std::string ticker);
        uint32_t wrapper_purchase_amount(std::string ticker, uint32_t amount);
        uint32_t wrapper_sell_amount(std::string ticker, uint32_t amount);
};























#endif //WRAPPER_CLASS