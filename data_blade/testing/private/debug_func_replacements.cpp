#include <iostream>
#include "debug_func_replacements.h"

//TODO make use of static vars for significant changes

//So far three types of debug funcs
//uint32_t debug_amount_owned_func(std::string)
//double debug_stock_price_func(std::string)
//double debug_account_cash_func()

//AMOUNT_OWNED
uint32_t debug_amount_owned_ZERO(std::string ticker) {
    return 0;
}

std::map<std::string, uint32_t> debug_amount_owned_GLOBAL_map;
uint32_t debug_amount_owned_GLOBAL(std::string ticker) {
    if (debug_amount_owned_GLOBAL_map.find(ticker) != debug_amount_owned_GLOBAL_map.end()) {
        return debug_amount_owned_GLOBAL_map[ticker];
    }
}

uint32_t debug_amount_owned_USER(std::string ticker) {
    uint32_t return_val;
    std::cout<<"Ticker: "<<ticker<<" is requesting amount_owned()."<<std::endl<<"Enter a valid number for amount_owned: ";
    std::cin>>return_val;
    return return_val;
}


//STOCK_PRICE
double debug_stock_price_ZERO(std::string ticker) {
    return 0;
}

std::map<std::string, double> debug_stock_price_GLOBAL_map;
double debug_stock_price_GLOBAL(std::string ticker) {
    if (debug_stock_price_GLOBAL_map.find(ticker) != debug_stock_price_GLOBAL_map.end()) {
        return debug_stock_price_GLOBAL_map[ticker];
    }
}

double debug_stock_price_USER(std::string ticker) {
    double return_val;
    std::cout<<"Ticker: "<<ticker<<" is requesting stock_price()."<<std::endl<<"Enter a valid number for stock_price: ";
    std::cin>>return_val;
    return return_val;
}


//ACCOUNT CASH
double debug_account_cash_ZERO() {
    return 0;
}

double debug_account_cash_GLOBAL_var;
double debug_account_cash_GLOBAL() {
    return debug_account_cash_GLOBAL_var;
}

double debug_account_cash_USER() {
    double return_val;
    std::cout<<"Account is requesting account_cash()."<<std::endl<<"Enter a valid number for account_cash: ";
    std::cin>>return_val;
    return return_val;
}

