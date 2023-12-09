#include "global_defines.h"
#include "debug_api.h"
#include "broker_api_class.h"

#ifndef DEBUG_API_AND_WRAPPER
#define DEBUG_API_AND_WRAPPER



#if DEBUG_API

#define AMOUNT_OWNED_CALL(ticker) debug_amount_owned(ticker);//THIS SHOULD BE AN STD::FUNC WHICH IS FOUND AS A GLOBAL VAR IN A TESTING FILE, THAT WAY IT CAN BE DEFINED AND REDEFINED AT WILL
#define STOCK_PRICE_CALL(ticker) debug_stock_price(ticker);//THIS SHOULD BE AN STD::FUNC WHICH IS FOUND AS A GLOBAL VAR IN A TESTING FILE, THAT WAY IT CAN BE DEFINED AND REDEFINED AT WILL
#define ACCOUNT_CASH debug_account_cash();


#else //Not DEBUG_API


#define AMOUNT_OWNED_CALL(ticker) _tied_account->get_wrapper_class()->wrapper_amount_owned(_ticker);
#define STOCK_PRICE_CALL(ticker) _tied_account->get_wrapper_class()->wrapper_stock_price(ticker);
#define ACCOUNT_CASH  = get_wrapper_class()->wrapper_account_cash();


#endif//Not either PROD or DEBUG_API
























#endif