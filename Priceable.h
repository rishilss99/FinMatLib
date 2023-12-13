#pragma once

#include "MultiStockModel.h"

/**
 *   Represents a general security
 */
class Priceable {
public:
    /*  Compute the price of the security  */
    virtual double price(
        const MultiStockModel& model ) const = 0;
};