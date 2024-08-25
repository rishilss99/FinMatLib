#pragma once

#include "stdafx.h"
#include "Priceable.h"
#include "Matrix.h"

/**
 *  Interface class for an option whose payoff should
 *  be approximated by looking at stock prices over all time
 *  time points
 */
class ContinuousTimeOption : public Priceable {
public: 
    /*  Virtual destructor */
    virtual ~ContinuousTimeOption() {};
    /*  The maturity of the option */
    virtual double getMaturity() const = 0;
    /*  Calculate the payoff of the option given
        a history of prices */
    virtual Matrix payoff(
        const MarketSimulation& simulation
        ) const = 0;
    /*  Is the option path-dependent?*/
    virtual bool isPathDependent() const = 0;
	/*  What stocks does the contract depend upon? */
	virtual std::set<std::string>
		getStocks() const = 0;
};

typedef std::shared_ptr<ContinuousTimeOption> SPContinuousTimeOption;
typedef std::shared_ptr<const ContinuousTimeOption> SPCContinuousTimeOption;