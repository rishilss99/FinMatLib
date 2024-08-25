#include "textfunctions.h"

using namespace std;

/**
 *  Replace all quote characters etc in a
 *  string with the correct escape characters
 *  to place into a Javascript string. So the text
 * 
 *  "Double quotes",'single quotes'
 *
 *  would become \"Double quotes\",\'single quotes\'.
 */
std::string escapeJavascriptString( const std::string& in ) {
    int n = in.size();
    stringstream s;
    for (int i=0; i<n; i++) {
        char c = in[i];
        if (c=='\'') {
            s<<"\\\'";
        } else if (c=='\"') {
            s<<"\\\"";
        } else if (c=='\\') {
            s<<"\\\\";
        } else if (c=='\t') {
            s<<"\\t";
        } else if (c=='\n') {
            s<<"\\n";
        } else if (c=='\r') {
            s<<"\\r";
        } else {
            s<<c;
        }
    }
    return s.str();
}


//
//   TESTS
//


static void testEscapeJavascriptString() {
    string in = "\"\'\\\r\n\tNot escaped";
    string out = "\\\"\\\'\\\\\\r\\n\\tNot escaped";
    ASSERT( escapeJavascriptString( in )==out );
}

void testTextFunctions() {
    TEST( testEscapeJavascriptString );
}