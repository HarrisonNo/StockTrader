#include <stdlib.h>//exit
#include <iostream>//cout
#include <assert.h>
#include "global_defines.h"

#ifndef ASSERT_AND_VERIFY
#define ASSERT_AND_VERIFY

//Prod is automatically calculated based on the values of PARTIAL_DEBUG and FULL_DEBUG, if neither are set then the assumption is that this is a PROD build
#define PROD 1


//Prod is determined by if we are going to crash on VERIFYs
#if VERIFY_CRASH
    #undef PROD
    #define PROD 0
#endif

//DEBUG is also automatically calculated, used for initializing vars for ASSERTS
#if PROD
    #define DEBUG 1
#else
    #define DEBUG 0
#endif

//May want to sub out definitions for printing and crashing later
//T represents failure type i.e. (ASSERT or VERIFY)
#define FAIL_PRINT(T, X) printf("Tripped "#T" in file: %s line: %d\n"#T":"#X,__FILE__,__LINE__); std::cout<<std::endl;

//TODODOD? https://en.cppreference.com/w/cpp/utility/source_location replace ASSERTS with that or with std::assert?
//https://stackoverflow.com/questions/2849832/c-c-line-number


//ASSERT and VERIFY definitions
#if VERBOSE
    #define ASSERT(X) if (!(X)) {FAIL_PRINT(ASSERT, X);}
#else
    #define ASSERT(X)
#endif

#if VERIFY_CRASH
    #define VERIFY(X) if (!(X)) {FAIL_PRINT(VERIFY, X); assert(X);}
#else //DO NOT CRASH ON VERIFY
    #define VERIFY(X) if (!(X)) {FAIL_PRINT(VERIFY, X);}
#endif











#endif//ASSERT_AND_VERIFY
