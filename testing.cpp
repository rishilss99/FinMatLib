#include "testing.h"

/*  Whether debug messages are enabled */
static bool debugEnabled = false;

bool isDebugEnabled() {
    return debugEnabled;
}

void setDebugEnabled( bool enable ) {
    debugEnabled = enable;
}

