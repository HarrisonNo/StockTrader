#include <iostream>
#include "debug_func_replacements.h"

//TODO make use of static vars for significant changes

#define DEFAULT_ACCOUNT_CASH_AMOUNT 0
#define DEFAULT_CURRENT_TIME 69420 //Important that this isn't zero as some calculations assume a start time greater than zero, oridinarily a completely safe assumption

//Global vars
std::map<std::string, uint32_t> debug_amount_owned_GLOBAL_map;
bool debug_amount_owned_NATURAL_CHANGING = true;
std::map<std::string, double> debug_stock_price_GLOBAL_map;
double debug_account_cash_GLOBAL_var = DEFAULT_ACCOUNT_CASH_AMOUNT;
bool debug_account_cash_NATURAL_CHANGING = true;
time_t debug_current_time_GLOBAL_var = DEFAULT_CURRENT_TIME;
time_t debug_current_time_ADDED_GLOBAL_var = 0;
bool debug_current_time_NATURAL_CHANGING = true;

//So far five types of debug funcs
//uint32_t debug_amount_owned_func(std::string)
//double debug_stock_price_func(std::string)
//double debug_account_cash_func()
//uint32_t debug_purchase_amount_func(uint32_t)
//uint32_t debug_sell_amount_func(uint32_t)

//AMOUNT_OWNED
uint32_t debug_amount_owned_GLOBAL(std::string ticker) {
    if (auto search = debug_amount_owned_GLOBAL_map.find(ticker); search != debug_amount_owned_GLOBAL_map.end()) {
        return search->second;
    }
    return 0;
}

void debug_amount_owned_SET_GLOBAL(std::string ticker, uint32_t set_amount) {
    debug_amount_owned_GLOBAL_map[ticker] = set_amount;
}

void debug_amount_owned_RESET_GLOBAL() {
    debug_amount_owned_GLOBAL_map.clear();
}

void debug_amount_owned_SET_NATURAL_CHANGING(bool enable_changing) {
    debug_amount_owned_NATURAL_CHANGING = enable_changing;
}

uint32_t debug_amount_owned_USER(std::string ticker) {
    uint32_t return_val;
    std::cout<<"Ticker: "<<ticker<<" is requesting amount_owned()."<<std::endl<<"Enter a valid number for amount_owned: ";
    std::cin>>return_val;
    return return_val;
}



//STOCK_PRICE
double debug_stock_price_GLOBAL(std::string ticker) {
    if (auto search = debug_stock_price_GLOBAL_map.find(ticker); search != debug_stock_price_GLOBAL_map.end()) {
        return search->second;
    }
    return 0;
}

void debug_stock_price_SET_GLOBAL(std::string ticker, double set_price) {
    debug_stock_price_GLOBAL_map[ticker] = set_price;
}

void debug_stock_price_RESET_GLOBAL() {
    debug_stock_price_GLOBAL_map.clear();
}

double debug_stock_price_USER(std::string ticker) {
    double return_val;
    std::cout<<"Ticker: "<<ticker<<" is requesting stock_price()."<<std::endl<<"Enter a valid number for stock_price: ";
    std::cin>>return_val;
    return return_val;
}



//ACCOUNT CASH
double debug_account_cash_GLOBAL() {
    return debug_account_cash_GLOBAL_var;
}

void debug_account_cash_SET_GLOBAL(double set_cash) {
    debug_account_cash_GLOBAL_var = set_cash;
}

void debug_account_cash_RESET_GLOBAL() {
    debug_account_cash_GLOBAL_var = DEFAULT_ACCOUNT_CASH_AMOUNT;
}

void debug_account_cash_SET_NATURAL_CHANGING(bool enable_changing) {
    debug_account_cash_NATURAL_CHANGING = enable_changing;
}

double debug_account_cash_USER() {
    double return_val;
    std::cout<<"Account is requesting account_cash()."<<std::endl<<"Enter a valid number for account_cash: ";
    std::cin>>return_val;
    return return_val;
}



//PURCHASE AMOUNT
uint32_t debug_purchase_amount_REQUESTED(std::string ticker, uint32_t requested_amount) {
    if (debug_amount_owned_NATURAL_CHANGING) {
        debug_amount_owned_SET_GLOBAL(ticker, debug_amount_owned_GLOBAL(ticker) + requested_amount);
    }
    if (debug_account_cash_NATURAL_CHANGING) {
        debug_account_cash_SET_GLOBAL(debug_account_cash_GLOBAL() - (requested_amount * debug_stock_price_GLOBAL(ticker)));
    }
    return requested_amount;
}



//SELL AMOUNT
uint32_t debug_sell_amount_REQUESTED(std::string ticker, uint32_t requested_amount) {
    if (debug_amount_owned_NATURAL_CHANGING) {
        debug_amount_owned_SET_GLOBAL(ticker, debug_amount_owned_GLOBAL(ticker) - requested_amount);
    }
    if (debug_account_cash_NATURAL_CHANGING) {
        debug_account_cash_SET_GLOBAL(debug_account_cash_GLOBAL() + (requested_amount * debug_stock_price_GLOBAL(ticker)));
    }
    return requested_amount;
}



//CURRENT TIME
time_t debug_current_time_GLOBAL() {
    if (debug_current_time_NATURAL_CHANGING) {
        return debug_current_time_GLOBAL_var + (time(NULL) - debug_current_time_ADDED_GLOBAL_var);
    }
    return debug_current_time_GLOBAL_var;
}

void debug_current_time_SET_GLOBAL(time_t set_time) {
    debug_current_time_GLOBAL_var = set_time;
}

void debug_current_time_RESET_GLOBAL() {
    debug_current_time_GLOBAL_var = DEFAULT_CURRENT_TIME;
    debug_current_time_RESET_ADDED_GLOBAL();
}

void debug_current_time_RESET_ADDED_GLOBAL() {
    debug_current_time_ADDED_GLOBAL_var = time(NULL);
}

void debug_current_time_SET_NATURAL_CHANGING(bool enable_changing) {
    debug_current_time_NATURAL_CHANGING = enable_changing;
}

time_t debug_current_time_REAL() {
    return time(NULL);
}


//SLEEP FOR
void debug_sleep_for_FAKE(time_t sleep_secs) {
    debug_current_time_SET_GLOBAL(debug_current_time_GLOBAL() + sleep_secs);
}