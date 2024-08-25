#pragma once

#include "stdafx.h"

// Functions for manipulating text

/**
 *  Replace all quote characters etc in a
 *  string with the correct escape characters
 *  to place into Javascript
 */
std::string escapeJavascriptString( const std::string& in );



void testTextFunctions();