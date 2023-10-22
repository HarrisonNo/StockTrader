#include <string>


#ifndef WRAPPER_API
#define WRAPPER_API

/*
Input:
Output:
Description:
Assumptions:
*/
double wrapper_stock_price(std::string ticker);


/*
Input:
Output:
Description:
Assumptions:
*/
uint32_t wrapper_amount_owned(std::string ticker);


/*
Input:
Output: number successfully purchased
Description:
Assumptions:
*/
uint32_t wrapper_purchase_amount(std::string ticker, uint32_t amount);

/*
Input:
Output: number successfully purchased
Description:
Assumptions:
*/
uint32_t wrapper_sell_amount(std::string ticker, uint32_t amount);


#endif WRAPPER_API