#include <stdlib.h>//exit
#include <iostream>//cout

//DO NOT TOUCH THESE, ALTER WITH SUB_PROGRAM COMPILE_TIME_SET_DEBUG
#include "FULL_DEBUG.h"
#include "PARTIAL_DEBUG.h"
#include "VERBOSE.h"
//DO NOT TOUCH THESE, ALTER WITH SUB_PROGRAM COMPILE_TIME_SET_DEBUG

#ifndef ASSERT_AND_VERIFY
#define ASSERT_AND_VERIFY


#if VERBOSE
#define DPRINT(X) std::cout<<X<<std::endl;
#endif

//Prod is automatically calculated based on the values of PARTIAL_DEBUG and FULL_DEBUG, if neither are set then the assumption is that this is a PROD build
#define PROD 1

#if FULL_DEBUG
#undef PROD
#define PROD 0
#endif

#if PARTIAL_DEBUG
#undef PROD
#define PROD 0
#endif

//DEBUG is also automatically calculated, used for initializing vars for ASSERTS
#if PROD
#define DEBUG 1
#else
#define DEBUG 0
#endif

//FULL_DEBUG takes priority over PARTIAL_DEBUG, and since only one is allowed at a time just disable PARTIAL_DEBUG
#if FULL_DEBUG
#if PARTIAL_DEBUG
#undef PARTIAL_DEBUG
#define PARTIAL_DEBUG 0
#endif
#endif

#if FULL_DEBUG
#undef NON_PROD
#define NON_PROD 1
#elif PARTIAL_DEBUG
#undef NON_PROD
#define NONE_PROD 1
#endif


//FULL_DEBUG treats all ASSERT and VERIFY trips as total failures, should only be used for testing with the expectation of unrecoverable failures
#if FULL_DEBUG
#define ASSERT(X) if (!X) {printf("Tripped ASSERT in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"ASSERT:"<<std::to_string(X)<<std::endl; std::exit(-1);}
#define VERIFY(X) if (!X) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<std::to_string(X)<<std::endl; std::exit(-1);}
#endif

//PARTIAL_DEBUG treats ASSERT trips as recoverable failures and VERIFY trips as uncrecoverable failures
#if PARTIAL_DEBUG
#define ASSERT(X) if (!X) {printf("Tripped ASSERT in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"ASSERT:"<<std::to_string(X)<<std::endl;}
#define VERIFY(X) if (!X) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<std::to_string(X)<<std::endl; std::exit(-1);}
#endif

//PROD treats ASSERT trips as acceptable pathways and VERIFY trips as recoverable failures(user is still notified via email/text TODO)
#if PROD
#define ASSERT(X) 
#define VERIFY(X) if (!X) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<std::to_string(X)<<std::endl;}
#endif



































#endif//ASSERT_AND_VERIFY
