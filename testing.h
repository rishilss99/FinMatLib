#pragma once

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cassert>

/*  Is debugging currently enabled */
bool isDebugEnabled();
/*  Enabled/disable debug */
void setDebugEnabled( bool enabled );

/*  Log an information statement */
#define INFO( A ) { \
    std::cerr << "INFO:\n" << __FILE__ <<":"<<__LINE__ << ":\n" << A <<"\n";\
}

#define TEST( f ) do {\
    std::cerr<<"Calling "<<#f<<"()\n"; \
    try { \
        f(); \
    } catch (...) { \
        std::cerr<<"\n"; \
        std::cerr<<"******* "<<#f<<"() FAILED. ********\n";\
        std::cerr<<"\n"; \
        exit(1); \
    }\
    std::cerr<<""<<#f<<"() passed.\n"; \
    std::cerr<<"\n"; \
} while (false)



// on windows we define debug mode to be when _DEBUG is set
#ifdef _DEBUG
#define DEBUG_MODE 1
#endif

// on unix we define debug mode to be when _GLIBCXX is set
#ifdef _GLIBCXX_DEBUG
#define DEBUG_MODE 1
#endif

#ifdef DEBUG
#define DEBUG_MODE 1
#endif



// our assertion macros behave differently in test mode
#ifndef DEBUG_MODE

#define DEBUG_PRINT( A )
#define ASSERT( c ) do {} while (0)
#define ASSERT_APPROX_EQUAL( x, y, tolerance ) do {} while (0)


#else
/*  Write A to std:cerr so long as debug is enabled */
#define DEBUG_PRINT( A ) { \
    if (isDebugEnabled()) { \
        std::cerr << "DEBUG:\n" << __FILE__ <<":"<<__LINE__ <<":\n"<< A <<"\n";\
    } \
}

#define ASSERT( c ) do { \
    if (!(c)) { \
        std::stringstream testing_ss_; \
		testing_ss_ << "ASSERTION FAILED \n"; \
		testing_ss_ << __FILE__ << ":" << __LINE__ << ":\n" << #c; \
		std::cerr << testing_ss_.str(); \
		throw std::runtime_error(testing_ss_.str()); \
    } \
} while (false)

#define ASSERT_APPROX_EQUAL( x, y, tolerance ) do {\
    if (!(fabs((x)-(y))<=(tolerance))) { \
		std::stringstream testing_ss_; \
		testing_ss_ << "ASSERTION FAILED \n"; \
		testing_ss_ << "Expected " << (x) << "\n"; \
		testing_ss_ << "Actual " << (y) << "\n"; \
		testing_ss_ << __FILE__ << ":" << __LINE__ << ":\n"; \
		std::cerr << testing_ss_.str(); \
		throw std::runtime_error(testing_ss_.str()); \
    } \
} while (false)


#endif



