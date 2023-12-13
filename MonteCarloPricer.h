#pragma once

#include "stdafx.h"
#include "ContinuousTimeOption.h"
#include "MultiStockModel.h"

class MonteCarloPricer {
public:
    /*  Constructor */
    MonteCarloPricer();
    /*  Number of scenarios */
    int nScenarios;
    /*  The number of steps in the calculation */
    int nSteps;
    /*  Price a path dependent option */
    double price( const ContinuousTimeOption& option,
                  const BlackScholesModel& model ) const;
	/*  Price a path dependent option */
	double price(const ContinuousTimeOption& option,
		const MultiStockModel& model) const;

};

void testMonteCarloPricer();

