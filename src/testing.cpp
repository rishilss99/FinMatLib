#include "testing.h"
#include "stdafx.h"

using namespace std;

/*  Whether debug messages are enabled */
static bool debugEnabled = false;
/* Mutex */
static mutex logMutex;

bool isDebugEnabled() {
	lock_guard<mutex> lock(logMutex);
    return debugEnabled;
}

void setDebugEnabled( bool enable ) {
	lock_guard<mutex> lock(logMutex);
    debugEnabled = enable;
}


/* Write a string stream to file. We know that ostream is a string stream */
void outputLog(const std::string& s) {
	lock_guard<mutex> lock(logMutex);
	std::cerr << s;
}
