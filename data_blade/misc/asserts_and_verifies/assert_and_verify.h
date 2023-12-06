#include <stdlib.h>//exit
#include <iostream>//cout
#include "global_defines.h"

#ifndef ASSERT_AND_VERIFY
#define ASSERT_AND_VERIFY

//Prod is automatically calculated based on the values of PARTIAL_DEBUG and FULL_DEBUG, if neither are set then the assumption is that this is a PROD build
#define PROD 1


//Prod is determined by if we are going to crash on either ASSERTs or VERIFYs
#if ASSERT_CRASH
    #undef PROD
    #define PROD 0
#endif

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



//ASSERT and VERIFY definitions
#if ASSERT_CRASH
    #define ASSERT(X) if (!(X)) {printf("Tripped ASSERT in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"ASSERT:"<<std::to_string(X)<<std::endl; std::exit(-1);}
#else //DO NOT CRASH ON ASSERT
    #if VERBOSE
        #define ASSERT(X) if (!(X)) {printf("Tripped ASSERT in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"ASSERT:"<<std::to_string(X)<<std::endl;}
    #else
        #define ASSERT(X)
    #endif
#endif

#if VERIFY_CRASH
    #define VERIFY(X) if (!(X)) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<std::to_string(X)<<std::endl; std::exit(-1);}
#else //DO NOT CRASH ON VERIFY
    #define VERIFY(X) if (!(X)) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<std::to_string(X)<<std::endl;}
#endif

//PROD treats ASSERT trips as acceptable pathways and VERIFY trips as recoverable failures(user is still notified via email/text TODO)
#if PROD
    #define ASSERT(X) 
    #define VERIFY(X) if (!(X)) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<std::to_string(X)<<std::endl;}
#endif



//DPRINT definiton
#if VERBOSE
    #define DPRINT(X) std::cout<<X<<std::endl;
#else
    #define DPRINT(X)
#endif











#endif//ASSERT_AND_VERIFY
