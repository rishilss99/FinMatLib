#include "testing.h"

/*  Whether debug messages are enabled */
static bool debugEnabled = false;

bool isDebugEnabled() {
    return debugEnabled;
}

void setDebugEnabled( bool enable ) {
    debugEnabled = enable;
}

/*  Demangle the typeid output of a given object */
std::string demangle(const char* name)
{
    std::unique_ptr<char, void(*)(void *)> res{abi::__cxa_demangle(name, NULL, NULL, NULL), std::free};
    return res != nullptr ? res.get() : name;
}
