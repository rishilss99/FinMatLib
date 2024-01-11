#pragma once

#include "ContinuousTimeOption.h"
#include "MonteCarloPricer.h"

/**
*   A Margrabe option which pays of the maximum of S_1-S_2 and 0.
*/
class MargrabeOption : public ContinuousTimeOption {
public:

	/*  The maturity of the option */
	virtual double getMaturity() const override {
		return maturity;
	}

	/*  What stocks does the contract depend upon? */
	virtual std::set<std::string>
		getStocks() const override;


	virtual Matrix payoff(
		const MarketSimulation& simulation
		) const override;

	bool isPathDependent() const override {
		return false;
	}

	double price(const MultiStockModel& model) const {
		MonteCarloPricer pricer;
		return pricer.price(*this, model);
	}


	std::string stock1;
	std::string stock2;
	double maturity;

};


void testMargrabeOption();