#include "global_defines.h"
#include "broker_api_class.h"

#ifndef DEBUG_API_AND_WRAPPER
#define DEBUG_API_AND_WRAPPER



#if DEBUG_API

#include "debug_api.h"//Include debug in if statement to not include global vars located inside

#define AMOUNT_OWNED_CALL(ticker) debug_amount_owned_func(ticker);
#define STOCK_PRICE_CALL(ticker) debug_stock_price_func(ticker);
#define ACCOUNT_CASH debug_account_cash_func();
#define PURCHASE_AMOUNT(ticker, amount) debug_purchase_amount_func(ticker, amount);
#define SELL_AMOUNT(ticker, amount) debug_sell_amount_func(ticker, amount);


#else //Not DEBUG_API

#define AMOUNT_OWNED_CALL(ticker) _tied_account->get_wrapper_class()->wrapper_amount_owned(_ticker);
#define STOCK_PRICE_CALL(ticker) _tied_account->get_wrapper_class()->wrapper_stock_price(ticker);
#define ACCOUNT_CASH  get_wrapper_class()->wrapper_account_cash();
#define PURCHASE_AMOUNT(ticker, amount) _tied_account->get_wrapper_class()->wrapper_purchase_amount(ticker, amount);
#define SELL_AMOUNT(ticker, amount) _tied_account->get_wrapper_class()->wrapper_sell_amount(ticker, amount);


#endif//Not either PROD or DEBUG_API
























#endif