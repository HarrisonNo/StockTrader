#include <string>
#include "broker_api_class.h"

/*
Input:
Output:
Description:
Assumptions:
*/
double wrapper_class::wrapper_stock_price(std::string ticker) {
    (void) ticker;//TEMP used to stop compiler warnings from unused variable
    return 0;
}


/*
Input:
Output:
Description:
Assumptions:
*/
uint32_t wrapper_class::wrapper_amount_owned(std::string ticker) {
    (void) ticker;//TEMP used to stop compiler warnings from unused variable
    return 0;
}


/*
Input:
Output: number successfully purchased
Description:
Assumptions:
*/
uint32_t wrapper_class::wrapper_purchase_amount(std::string ticker, uint32_t amount) {
    (void) ticker;//TEMP used to stop compiler warnings from unused variable
    (void) amount;
    return 0;
}

/*
Input:
Output: number successfully purchased
Description:
Assumptions:
*/
uint32_t wrapper_class::wrapper_sell_amount(std::string ticker, uint32_t amount) {
    (void) ticker;//TEMP used to stop compiler warnings from unused variable
    (void) amount;
    return 0;
}


/*
Input:
Output: number successfully purchased
Description:
Assumptions:
*/
double wrapper_class::wrapper_account_cash() {
    return 0;
}