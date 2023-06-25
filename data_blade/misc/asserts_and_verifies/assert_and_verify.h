#ifndef ASSERT_AND_VERIFY
#define ASSERT_AND_VERIFY

#include <stdlib.h>//exit
#include <iostream>//cout

//DO NOT TOUCH THESE, ALTER WITH SUB_PROGRAM COMPILE_TIME_SET_DEBUG
#include "FULL_DEBUG.h"
#include "DEBUG.h"
#include "VERBOSE.h"
//DO NOT TOUCH THESE, ALTER WITH SUB_PROGRAM COMPILE_TIME_SET_DEBUG

#if VERBOSE
#define DPRINT(X) std::cout<<X<<std::endl;
#endif

//Prod is automatically calculated based on the values of DEBUG and FULL_DEBUG, if neither are set then the assumption is that this is a PROD build
#define PROD 1

#if FULL_DEBUG
#undef PROD
#define PROD 0
#endif

#if DEBUG
#undef PROD
#define PROD 0
#endif

//FULL_DEBUG takes priority over DEBUG, and since only one is allowed at a time just disable DEBUG
#if FULL_DEBUG
#if DEBUG
#undef DEBUG
#define DEBUG 0
#endif
#endif

//FULL_DEBUG treats all ASSERT and VERIFY trips as total failures, should only be used for testing with the expectation of unrecoverable failures
#if FULL_DEBUG
#define ASSERT(X) if (!X) {printf("Tripped ASSERT in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"ASSERT:"<<X<<std::endl; std::exit(-1);}
#define VERIFY(X) if (!X) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<X<<std::endl; std::exit(-1);}
#endif

//DEBUG treats ASSERT trips as recoverable failures and VERIFY trips as uncrecoverable failures
#if DEBUG
#define ASSERT(X) if (!X) {printf("Tripped ASSERT in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"ASSERT:"<<X<<std::endl;}
#define VERIFY(X) if (!X) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<X<<std::endl;std::exit(-1);}
#endif

//PROD treats ASSERT trips as acceptable pathways and VERIFY trips as recoverable failures(user is still notified via email/text TODO)
#if PROD
#define ASSERT(X) 
#define VERIFY(X) if (!X) {printf("Tripped VERIFY in file: %s line: %d\n",__FILE__,__LINE__); std::cout<<"VERIFY:"<<X<<std::endl;}
#endif


































#endif//ASSERT_AND_VERIFY
