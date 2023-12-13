#pragma once

#include <vector>
#include <list>
#include <string>

class HashMap
{
public:
    HashMap();
    size_t getNumBuckets()
    {
        return buckets;
    }
    void store(std::string key, std::string value);
    std::string get(std::string &key);

private:
    typedef std::pair<std::string, std::string> hashObj;

    std::list<hashObj>::iterator getIterator(std::string &key);
    size_t hashIndex(std::string &key);
    size_t buckets;
    std::vector<std::list<hashObj>> hashTable;
};

void testHashMap();