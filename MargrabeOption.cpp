#include "MargrabeOption.h"
#include "MonteCarloPricer.h"

#include "matlib.h"

MargrabeOption::MargrabeOption() : stockPair({"Acme", "Chumhum"}), maturity(1.0) {}

/*  Implementation based off Wikipedia solution  */
double MargrabeOption::price(const MultiStockModel &msm) const
{
    BlackScholesModel bsm1 = msm.getBlackScholesModel(stockPair.first);
    BlackScholesModel bsm2 = msm.getBlackScholesModel(stockPair.second);

    double S1 = bsm1.stockPrice;
    double S2 = bsm2.stockPrice;
    double sigma1 = bsm1.volatility;
    double sigma2 = bsm2.volatility;
    double p = msm.getCovarianceStockPair(stockPair.first, stockPair.second) / (sigma1 * sigma2);  // Pearson's correlation coefficient
    double T = getMaturity() - msm.getDate();

    double sigma = sqrt(sigma1 * sigma1 + sigma2 * sigma2 - 2.0 * sigma1 * sigma2 * p);

    double numerator = log(S1 / S2) + (sigma * sigma * 0.5) * T;
    double denominator = sigma * sqrt(T);
    double d1 = numerator / denominator;
    double d2 = d1 - denominator;
    return S1 * normcdf(d1) - S2 * normcdf(d2);
}

Matrix MargrabeOption::payoff(const MarketSimulation &sim) const
{
    Matrix val = getPriceAtMaturity(*sim.getStockPaths(stockPair.first)) - getPriceAtMaturity(*sim.getStockPaths(stockPair.second));
    val.positivePart();
    return val;
}

//////////////////////////
//
//  Test the call option class
//
//
//////////////////////////

static void testMargrabeOptionPrice()
{
    MultiStockModel msm = MultiStockModel::createTestModel();

    MargrabeOption margrabeOption;
    margrabeOption.setStocks("Bigbank", "Acme");
    margrabeOption.setMaturity(3);
    double price = margrabeOption.price(msm);

    MonteCarloPricer pricer;
    double expected = pricer.price(margrabeOption, msm);
    setDebugEnabled(true);
    DEBUG_PRINT(expected);
    setDebugEnabled(true);
    ASSERT_APPROX_EQUAL(price, expected, 1);
}

void testMargrabeOption()
{
    TEST(testMargrabeOptionPrice);
}
