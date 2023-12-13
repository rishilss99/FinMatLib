#pragma once

#include "stdafx.h"
#include "MultiStockModel.h"
#include "ContinuousTimeOption.h"

class MargrabeOption : public ContinuousTimeOption
{
public:
    MargrabeOption();

    /*  What stocks does the contract depend upon? */
    std::set<std::string> getStocks() const
    {
        auto stockSet = std::set<std::string>({stockPair.first, stockPair.second});
        ASSERT(stockSet.size() == 2);
        return stockSet;
    }

    void setStocks(std::string stock1, std::string stock2)
    {
        ASSERT(stock1 != stock2);
        stockPair = {stock1, stock2};
    }

    double getMaturity() const
    {
        return maturity;
    }

    void setMaturity(double maturity)
    {
        this->maturity = maturity;
    }

    /*  Price the option using the analytical formula */
    double price(const MultiStockModel &msm) const;

    /*  Returns the price at maturity from a price path */
    Matrix getPriceAtMaturity(const Matrix &stockPrices) const
    {
        return stockPrices.col(stockPrices.nCols() - 1);
    }

    /*  Compute the payoff given the a simulation of the market */
    Matrix payoff(const MarketSimulation &sim) const;

    /*  Is the option path-dependent?*/
    bool isPathDependent() const
    {
        return false;
    }

private:
    std::pair<std::string, std::string> stockPair;
    double maturity;
};

void testMargrabeOption();
