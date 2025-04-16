#ifndef ASSERTIONS_HPP
#define ASSERTIONS_HPP

#include "utilities/Logger.hpp"

#ifndef DEBUG_MODE

// No-op in release mode
#define ASSERT(condition) ((void)0)
#define ASSERT_FALSE() ((void)0)
#define STATIC_ASSERT(condition, message) ((void)0)
#define STATIC_ASSERT_FALSE(condition, message) ((void)0)

#else

// Evaluate condition only once
#define ASSERT(condition) do { \
            bool cond = (condition); \
            if(!cond) { \
                LOG_CRITICAL("Assertion failed {}", #condition); \
                assert(false); \
            } \
        } while (0) 

#define ASSERT_FALSE() do { \
            LOG_CRITICAL("False assertion reached"); \
            assert(false); \
        } while (0) 

// Compile-time assertion
#define STATIC_ASSERT(condition, message) static_assert(condition, message)
#define STATIC_ASSERT_FALSE(message) static_assert(false, message)

#endif // DEBUG_MODE

#endif // !ASSERTIONS_HPP