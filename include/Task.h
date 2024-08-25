#pragma once

#include "stdafx.h"

/**
 *   A task is simply something with an execute
 *   method.
 */
class Task {
public:
    virtual ~Task() {}
    virtual void execute() = 0;
};
