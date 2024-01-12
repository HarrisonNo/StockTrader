#include <iostream>
#include "debug_func_replacements.h"
#include "debug_api.h"


#define DEFAULT_ACCOUNT_CASH_AMOUNT 0
#define DEFAULT_CURRENT_TIME 69420 //Important that this isn't zero as some calculations assume a start time greater than zero, oridinarily a completely safe assumption
#define DEFAULT_RNG 0

//Global vars
std::map<std::string, uint32_t> debug_amount_owned_GLOBAL_map;
bool debug_amount_owned_NATURAL_CHANGING = true;
std::map<std::string, double> debug_stock_price_GLOBAL_map;
double debug_account_cash_GLOBAL_var = DEFAULT_ACCOUNT_CASH_AMOUNT;
bool debug_account_cash_NATURAL_CHANGING_GLOBAL_var = true;
time_t debug_current_time_GLOBAL_var = DEFAULT_CURRENT_TIME;
time_t debug_current_time_ADDED_GLOBAL_var = 0;
bool debug_current_time_NATURAL_CHANGING_GLOBAL_var = true;
uint64_t debug_rng_var = DEFAULT_RNG;
std::map<std::string, std::map<double, uint32_t>> debug_historical_price_GLOBAL_map;//map with a key of the ticker string, yielding a second map storing the <price, amount_owned> //TODO add and remove when buying/selling

//So far five types of debug funcs
//uint32_t debug_amount_owned_func(std::string)
//double debug_stock_price_func(std::string)
//double debug_account_cash_func()
//uint32_t debug_purchase_amount_func(uint32_t)
//uint32_t debug_sell_amount_func(uint32_t)

//AMOUNT_OWNED
uint32_t debug_amount_owned_GLOBAL(std::string ticker) {
    uint32_t owned = 0;
    if (auto search = debug_amount_owned_GLOBAL_map.find(ticker); search != debug_amount_owned_GLOBAL_map.end()) {
        owned = search->second;
        debug_ADJUST_RNG(owned);
    }
    return owned;
}

//Has to be signed to allow for accidentally sending a less than zero value
void debug_amount_owned_SET_GLOBAL(std::string ticker, int64_t set_amount) {

    if (set_amount < 0) {
        throw("ERROR: debug_amount_owned_SET_GLOBAL was set to a value lower than zero\n");
    }

    debug_amount_owned_GLOBAL_map[ticker] = set_amount;
}

void debug_amount_owned_RESET_GLOBAL() {
    debug_amount_owned_GLOBAL_map.clear();
}

void debug_amount_owned_SET_NATURAL_CHANGING_GLOBAL(bool enable_changing) {
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
    double price = 0;
    if (auto search = debug_stock_price_GLOBAL_map.find(ticker); search != debug_stock_price_GLOBAL_map.end()) {
        price = search->second;
        debug_ADJUST_RNG(price);
    }
    return price;
}

void debug_stock_price_SET_GLOBAL(std::string ticker, double set_price) {

    if (set_price < 0) {
        throw("ERROR: debug_stock_price_SET_GLOBAL was set to a value lower than zero\n");//A buy triggered causing us to go over our alloted cash amount
    }

    debug_stock_price_GLOBAL_map[ticker] = set_price;
    debug_current_time_SET_GLOBAL(debug_current_time_GLOBAL() + 1);//Price changes cause a 1 second shift
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
    debug_ADJUST_RNG(1);
    return debug_account_cash_GLOBAL_var;
}

void debug_account_cash_SET_GLOBAL(double set_cash) {
    
    if (set_cash < 0) {
        throw("ERROR: debug_account_cash_SET_GLOBAL was set to a value lower than zero\n");//A buy triggered causing us to go over our alloted cash amount
    }

    debug_account_cash_GLOBAL_var = set_cash;
}

void debug_account_cash_RESET_GLOBAL() {
    debug_account_cash_GLOBAL_var = DEFAULT_ACCOUNT_CASH_AMOUNT;
}

void debug_account_cash_SET_NATURAL_CHANGING_GLOBAL(bool enable_changing) {
    debug_account_cash_NATURAL_CHANGING_GLOBAL_var = enable_changing;
}

double debug_account_cash_USER() {
    double return_val;
    std::cout<<"Account is requesting account_cash()."<<std::endl<<"Enter a valid number for account_cash: ";
    std::cin>>return_val;
    return return_val;
}



//PURCHASE AMOUNT
uint32_t debug_purchase_amount_REQUESTED(std::string ticker, uint32_t requested_amount) {
    double initial_account_cash = debug_account_cash_func();
    double initial_stock_price = debug_stock_price_func(ticker);

    debug_ADJUST_RNG(1);

    if (initial_account_cash < (requested_amount * debug_amount_owned_func(ticker))) {
        throw("ERROR: debug_purchase_amount_REQUESTED more shares than can be bought with current funds\n");
    }
    
    if (debug_amount_owned_NATURAL_CHANGING) {
        debug_amount_owned_SET_GLOBAL(ticker, debug_amount_owned_GLOBAL(ticker) + requested_amount);
    }

    if (debug_account_cash_NATURAL_CHANGING_GLOBAL_var) {
        debug_account_cash_SET_GLOBAL(debug_account_cash_GLOBAL() - (requested_amount * debug_stock_price_GLOBAL(ticker)));
    }

    auto search_map = debug_historical_price_GLOBAL_map.find(ticker);
    std::map<double, uint32_t> * historical_prices_map;

    if (search_map != debug_historical_price_GLOBAL_map.end()) {
        historical_prices_map = &(search_map->second);
    } else {
        debug_historical_price_GLOBAL_map[ticker] = {};
        historical_prices_map = &(debug_historical_price_GLOBAL_map[ticker]);
    }

    if (historical_prices_map->find(initial_stock_price) != historical_prices_map->end()) {//There is already an entry at this price point
        (*historical_prices_map)[initial_stock_price] += requested_amount;
    } else {//There is not an entry yet
        (*historical_prices_map)[initial_stock_price] = requested_amount;
    }

    return requested_amount;
}



//SELL AMOUNT
uint32_t debug_sell_amount_REQUESTED(std::string ticker, uint32_t requested_amount) {
    uint32_t already_owned_amount = debug_amount_owned_func(ticker);
    
    debug_ADJUST_RNG(1);

    if (requested_amount > already_owned_amount) {
        throw("ERROR: debug_sell_amount_REQUESTED a larger number than is currently held\n");
    }

    if (debug_amount_owned_NATURAL_CHANGING) {
        debug_amount_owned_SET_GLOBAL(ticker, already_owned_amount - requested_amount);
    }

    if (debug_account_cash_NATURAL_CHANGING_GLOBAL_var) {
        debug_account_cash_SET_GLOBAL(debug_account_cash_GLOBAL() + (requested_amount * debug_stock_price_GLOBAL(ticker)));
    }
    
    uint32_t amount_removed_from_map = 0;
    auto search = debug_historical_price_GLOBAL_map.find(ticker);
    std::map<double, uint32_t> * historical_prices_map;

    if (search != debug_historical_price_GLOBAL_map.end()) {
        historical_prices_map = &(search->second);
    } else {
        debug_historical_price_GLOBAL_map[ticker] = {};
        historical_prices_map = &(debug_historical_price_GLOBAL_map[ticker]);
    }

    for (auto it = historical_prices_map->begin(); it != historical_prices_map->end(); /*This is intentionally left blank as an exercise for the reader*/) {//ITERATE THROUGH MAP
        if (it->second <= (requested_amount - amount_removed_from_map)) {
            //Will pop this entry
            amount_removed_from_map += it->second;
            historical_prices_map->erase(it++);
        } else {
            //Will not pop this entry
            it->second -= (requested_amount - amount_removed_from_map);
            amount_removed_from_map = requested_amount;
        }

        if (amount_removed_from_map == requested_amount) {
            break;
        }
    }

    if (amount_removed_from_map < requested_amount) {
        throw("ERROR: debug_sell_amount_REQUESTED unable to remove enough entries from historical map\n");
    }

    return requested_amount;
}



//CURRENT TIME
time_t debug_current_time_GLOBAL() {
    //debug_ADJUST_RNG(1);//Likely do not want to tie RNG to time as that may also change based on program performance
    if (debug_current_time_NATURAL_CHANGING_GLOBAL_var) {
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

void debug_current_time_SET_NATURAL_CHANGING_GLOBAL(bool enable_changing) {
    debug_current_time_NATURAL_CHANGING_GLOBAL_var = enable_changing;
}

time_t debug_current_time_REAL() {
    return time(NULL);
}



//RNG
//fake rng is needed as I need the unit test to be reliably repeatable
uint64_t debug_RNG() {
    debug_ADJUST_RNG(1);
    return debug_rng_var;
}

void debug_ADJUST_RNG(uint64_t addition) {
    if ((UINT64_MAX - debug_rng_var) <= addition) {//Addition would push us over the edge
        debug_RESET_RNG();
        debug_rng_var += 1;
    }
    debug_rng_var += addition;
    return;
}

void debug_RESET_RNG() {
    debug_rng_var = DEFAULT_RNG;
    return;
}

uint64_t debug_RNG_GENERATE(uint64_t max) {
    return ((debug_rng_var++) % max);//Generates a num between 0-max
}

//both coefficients should be greater than or equal to 1
bool debug_RNG_GENERATE_ODDS(uint32_t positive_coefficient, uint32_t negative_cofficient) {
    uint64_t total_coefficient = positive_coefficient + negative_cofficient;
    uint64_t random_number = debug_RNG_GENERATE(total_coefficient);
    debug_ADJUST_RNG(random_number);

    if (random_number <= (positive_coefficient - 1)) {
        return true;
    }

    return false;
}



//RNG PRICE GENERATION
double debug_PRICE_RNG_generate_new_price(std::string ticker, uint32_t positive_price_coefficient, uint32_t negative_price_cofficient,
                                        /*percents should come in the form of 1.5, .8, 3.4*/
                                          double average_pos_percent_change, double max_pos_percent_change,
                                          double average_neg_percent_change, double max_neg_percent_change) {
    uint64_t average_percent_change, max_percent_change;
    bool is_price_increasing = debug_RNG_GENERATE_ODDS(positive_price_coefficient, negative_price_cofficient);
    //This dictates whether we are generating a number between 0-average or average-max
    bool second_roll = debug_RNG_GENERATE_ODDS((max_pos_percent_change * 1000) + (average_pos_percent_change * 1000), (max_neg_percent_change * 1000) + (average_neg_percent_change * 1000));//This is weirdly fucked, but it effectively takes the average
    double second_roll_mod = 0, original_price = debug_stock_price_GLOBAL(ticker), new_price, final_percent_change;

    //Multiply by 1000 to get rid of decimals before calculating the random number
    if (is_price_increasing) {
        average_percent_change = average_pos_percent_change * 1000;
        max_percent_change = max_pos_percent_change * 1000;
        if (second_roll) {
            second_roll_mod = average_percent_change;
            //std::cout<<"!ADDED"<<std::endl;
        }
    } else {
        average_percent_change = average_neg_percent_change * 1000;
        max_percent_change = max_neg_percent_change * 1000;
        if (!second_roll) {
            second_roll_mod = average_percent_change;
            //std::cout<<"!SUBTRACTED"<<std::endl;
        }
    }

    //We need to apply extra, as previously calculated. The method for generating the random number is different as a result though
    if (second_roll_mod) {
        final_percent_change = debug_RNG_GENERATE(max_percent_change - average_percent_change);
        final_percent_change += average_percent_change;
    } else {
        final_percent_change = debug_RNG_GENERATE(average_percent_change);
    }

    final_percent_change = final_percent_change / 1000;//Drop it back down to a real percent
    final_percent_change = final_percent_change / 100;//Then drop it down from something like 1.5 to .015

    if (is_price_increasing) {
        //std::cout<<"Increasing percent by "<<final_percent_change<<std::endl;
        new_price = original_price * (1 + final_percent_change); 
    } else {
        //std::cout<<"Decreasing percent by "<<final_percent_change<<std::endl;
        new_price = original_price * (1 - final_percent_change);
    }

    //debug_stock_price_SET_GLOBAL(ticker, new_price);//May not want to auto set new price
    
    if (debug_stock_price_GLOBAL(ticker) <= 1) {
        std::cout<<"WARNING function seed is leading to a stock price of lower than $1, changing seeds or coefficients is recommended"<<std::endl;
    }

    return new_price;
}



//SLEEP FOR
void debug_sleep_for_FAKE(time_t sleep_secs) {
    debug_current_time_SET_GLOBAL(debug_current_time_GLOBAL() + sleep_secs);
}



//HISTORICAL PRICE
void debug_historical_price_RESET_GLOBAL() {
    debug_historical_price_GLOBAL_map.clear();
}

uint32_t debug_historical_price_PROFITABLE_GLOBAL(std::string ticker, double price) {
    std::map<double, uint32_t> * map_ptr;
    auto search = debug_historical_price_GLOBAL_map.find(ticker);
    uint32_t total_sellable = 0;

    if (search != debug_historical_price_GLOBAL_map.end()) {
        map_ptr = &(search->second);
    } else {
        debug_historical_price_GLOBAL_map[ticker] = {};
        map_ptr = &(debug_historical_price_GLOBAL_map[ticker]);
    }

    for (auto it = map_ptr->begin(); it != map_ptr->end(); it++) {//ITERATE THROUGH MAP
        if (it->first <= price) {
            total_sellable += it->second;
        } else {
            break;
        }
    }

    return total_sellable;
}