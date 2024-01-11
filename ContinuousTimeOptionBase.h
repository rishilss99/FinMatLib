#pragma once

#include "ContinuousTimeOption.h"
#include "MultiStockModel.h"

/**
 *  Convenience class for eliminating the drudgery of
 *  writing option classes
 */
class ContinuousTimeOptionBase : public ContinuousTimeOption {
public:

	ContinuousTimeOptionBase() :
		stock(MultiStockModel::DEFAULT_STOCK),
		maturity(1.0),
		strike(0.0) {}

    virtual ~ContinuousTimeOptionBase() {}

	std::string getStock() const {
		return stock;
	}

	void setStock(std::string stock) {
		this->stock = stock;
	}

    double getMaturity() const {
        return maturity;
    }

    void setMaturity( double maturity ) {
        this->maturity = maturity;
    }

    double getStrike() const {
        return strike;
    }
    
    void setStrike( double strike ) {
        this->strike = strike;
    }

    /*  
     *  Convenience method to calculate an approximate price
     *  for the option using the most appropriate method for
     *  the given option. Note that since you can't control
     *  the accuracy of the calculation this isn't a good method
     *  for general use, but is handy for tests.
     */
    virtual double price( const MultiStockModel& model ) const;

	/**
	*  Compute the payoff given the prices for the stock
	*/
	virtual Matrix payoff(const Matrix& stockPrices) const = 0;

	/**
	*  Compute the payoff given the a simulation of the market
	*/
	Matrix payoff(const MarketSimulation& sim) const {
		return payoff(*sim.getStockPrices(getStock()));
	}

	/*  What stocks does the contract depend upon */
	std::set<std::string>
		getStocks() const {
		return std::set<std::string>({ getStock() });
	}

private:
	std::string stock;
    double maturity;
    double strike;
};

////////////////


void testContinuousTimeOptionBase();
