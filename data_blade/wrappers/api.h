#include "global_defines.h"
#include "broker_api_class.h"
#include "debug_api.h"

#ifndef DEBUG_API_AND_WRAPPER
#define DEBUG_API_AND_WRAPPER



#if DEBUG_API

//THESE CAN "FAIL"(force a throw)
#define AMOUNT_OWNED(receiver_var, ticker, ACTIONS) try {receiver_var = debug_amount_owned_func(ticker);} catch (...) {ASSERT(!"AMOUNT_OWNED wrapper failed"); ACTIONS}
#define STOCK_PRICE(receiver_var, ticker, ACTIONS) try {receiver_var = debug_stock_price_func(ticker);} catch (...) {ASSERT(!"STOCK_PRICE wrapper failed"); ACTIONS}
#define ACCOUNT_CASH(receiver_var, ACTIONS) try {receiver_var = debug_account_cash_func();} catch (...) {ASSERT(!"ACCOUNT_CASH wrapper failed"); ACTIONS}
#define PURCHASE_AMOUNT(receiver_var, ticker, amount, ACTIONS) try {receiver_var = debug_purchase_amount_func(ticker, amount);} catch (...) {ASSERT(!"PURCHASE_AMOUNT wrapper failed"); ACTIONS}
#define SELL_AMOUNT(receiver_var, ticker, amount, ACTIONS) try {receiver_var = debug_sell_amount_func(ticker, amount);} catch (...) {ASSERT(!"SELL_AMOUNT wrapper failed"); ACTIONS}
//THESE CANNOT "FAIL"
#define CURRENT_TIME() debug_current_time_func();
#define SLEEP_FOR(seconds) debug_sleep_for_func(seconds);


#else //Not DEBUG_API

#define AMOUNT_OWNED_CALL(receiver_var, ticker, ACTIONS) try {receiver_var = _tied_account->get_wrapper_class()->wrapper_amount_owned(_ticker);} catch (...) {ASSERT(!"AMOUNT_OWNED wrapper failed"); ACTIONS}
#define STOCK_PRICE_CALL(receiver_var, ticker, ACTIONS) try {receiver_var = _tied_account->get_wrapper_class()->wrapper_stock_price(ticker);} catch (...) {ASSERT(!"STOCK_PRICE wrapper failed"); ACTIONS}
#define ACCOUNT_CASH(receiver_var, ACTIONS)  try {receiver_var = get_wrapper_class()->wrapper_account_cash();} catch (...) {ASSERT(!"ACCOUNT_CASH wrapper failed"); ACTIONS}
#define PURCHASE_AMOUNT(receiver_var, ticker, amount, ACTIONS) try {receiver_var = _tied_account->get_wrapper_class()->wrapper_purchase_amount(ticker, amount);} catch (...) {ASSERT(!"PURCHASE_AMOUNT wrapper failed"); ACTIONS}
#define SELL_AMOUNT(receiver_var, ticker, amount, ACTIONS) try {receiver_var = _tied_account->get_wrapper_class()->wrapper_sell_amount(ticker, amount);} catch (...) {ASSERT(!"SELL_AMOUNT wrapper failed"); ACTIONS}
#define CURRENT_TIME() time(NULL);
//should sleep_for real func be a part of time_keeper?


#endif//Not either PROD or DEBUG_API
























#endif