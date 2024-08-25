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
	/*  The number of concurrent tasks to run */
	int nTasks;
    /*  Price a path dependent option */
    double price( const ContinuousTimeOption& option,
                  const BlackScholesModel& model ) const;
	/*  Price a path dependent option */
	double price(const ContinuousTimeOption& option,
		const MultiStockModel& model) const;
};

void testMonteCarloPricer();

