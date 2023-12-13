#pragma once

#include "stdafx.h"
#include "MultiStockModel.h"
#include "PathIndependentOption.h"

class CallOption : public PathIndependentOption {
public:

    /*  Returns the payoff at maturity given a column vector
        of scenarios */
    Matrix payoffAtMaturity( const Matrix& stockAtMaturity ) const;

    double price( const MultiStockModel& bsm )
        const;
};

void testCallOption();