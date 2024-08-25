#pragma once

#include "stdafx.h"

class MyObject
{
public:
    MyObject();
    MyObject(const MyObject &other);
    ~MyObject();
    MyObject &operator=(const MyObject &other);
    std::vector<int> &getVector()
    {
        return *(ptr.get());
    }

private:
    std::shared_ptr<std::vector<int>> ptr;
};

MyObject operator+(MyObject &a, MyObject &b);

void testMyObject();