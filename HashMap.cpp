#include "HashMap.h"
#include "stdafx.h"

using namespace std;

HashMap::HashMap() : buckets(26), hashTable(26) {}

size_t HashMap::hashIndex(string &key)
{
    ASSERT(key.length() != 0);
    size_t idx = tolower(key.front()) - 'a';
    return idx;
}

list<typename HashMap::hashObj>::iterator HashMap::getIterator(std::string &key)
{
    size_t idx = hashIndex(key);
    list<hashObj>::iterator itr = hashTable[idx].begin();
    while (itr != hashTable[idx].end())
    {
        if (itr->first == key)
        {
            return itr;
        }
        itr++;
    }
    throw invalid_argument("Given key not valid");
}

string HashMap::get(string &key)
{
    try
    {
        list<hashObj>::iterator itr = getIterator(key);
        return itr->second;
    }
    catch(const std::exception& e)
    {
        throw e;
    }
    
}

void HashMap::store(string key, string value)
{
    size_t idx = hashIndex(key);
    try
    {
        list<hashObj>::iterator itr = getIterator(key);
        itr->second = value;
    }
    catch (const exception &e)
    {
        hashTable[idx].push_back({key, value});
    }
}

static void testValidElementGet()
{
    HashMap h;
    h.store("Hey", "Hello");
    h.store("Nice", "There"); 
    string a = "Hey";
    ASSERT(h.get(a) == "Hello");
}

static void testInvalidElementGet()
{
    HashMap h;
    h.store("Hey", "Hello");
    h.store("Nice", "There"); 
    string a = "Ney";
    try
    {
        h.get(a);
    }
    catch(const std::exception& e)
    {
        ASSERT(e.what());
    }
}

void testHashMap()
{
    TEST(testValidElementGet);
    TEST(testInvalidElementGet);
}