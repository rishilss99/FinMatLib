#pragma once

#include "stdafx.h"
#include "ContinuousTimeOptionBase.h"

/**
 *   This states that all path independent options
 *   have a payoff determined by the final stock price
 */
class PathIndependentOption :
        public ContinuousTimeOptionBase {
public:
    /*  A virtual destructor */
    virtual ~PathIndependentOption() {}
    /*  Returns the payoff at maturity given a column vector
        of scenarios */
    virtual Matrix payoffAtMaturity( const Matrix& finalStockPrice) const
        = 0;
    /*  Compute the payoff from a price path */   
    Matrix payoff(
            const Matrix& stockPrices ) const {
        return payoffAtMaturity( stockPrices.col( stockPrices.nCols()-1 ) );                
    }
    /*  Is the option path dependent? */
    bool isPathDependent() const {
        return false;
    };
};

std::ostream &operator<<(std::ostream &out, const PathIndependentOption &option);