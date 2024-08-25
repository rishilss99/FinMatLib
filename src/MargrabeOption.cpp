#include "MargrabeOption.h"

#include "stdafx.h"
#include "testing.h"
#include "matlib.h"

using namespace std;

/*  What stocks does the contract depend upon? */
set<string> MargrabeOption::getStocks() const {
	return std::set<std::string>({ stock1, stock2 });
}

Matrix MargrabeOption::payoff(
	const MarketSimulation& simulation
	) const {
	auto stockPrices1 = simulation.getStockPrices(stock1);
	int nSteps = stockPrices1->nCols();
	Matrix finalPrices1 = stockPrices1->col(nSteps - 1);
	auto stockPrices2 = simulation.getStockPrices(stock2);
	Matrix finalPrices2 = stockPrices2->col(nSteps - 1);
	Matrix ret = finalPrices1 - finalPrices2;
	ret.positivePart();
	return ret;
}


static void testAnalyticalFormula() {

	rng("default");

	MargrabeOption m;
	m.stock1 = "Stock1";
	m.stock2 = "Stock2";
	m.maturity = 1.0;

	vector< string> stocks({ m.stock1, m.stock2 });
	Matrix stockPrices("100.0; 99.0");
	Matrix drifts("0.0; 0.05");
	Matrix covarianceMatrix("0.1,0.05;0.05,0.2");


	MultiStockModel model(stocks,
		stockPrices,
		drifts,
		covarianceMatrix);
	model.setRiskFreeRate(0.05);
	MonteCarloPricer pricer;
	pricer.nScenarios = 1000000;
	double monteCarloPrice = pricer.price(m, model);

	double sigma1 = sqrt(covarianceMatrix(0, 0));
	double sigma2 = sqrt(covarianceMatrix(1, 1));
	double rho = covarianceMatrix(0, 1) / (sigma1*sigma2);
	double S1 = stockPrices(0);
	double S2 = stockPrices(1);
	double T = m.maturity;

	double sigma = sqrt(sigma1*sigma1 + sigma2*sigma2 - 2 * sigma1*sigma2*rho);
	double d1 = (log(S1 / S2) + (sigma*sigma / 2)*T) / (sigma*sqrt(T));
	double d2 = d1 - sigma*sqrt(T);

	double analyticalPrice = S1*normcdf(d1) - S2*normcdf(d2);
	ASSERT_APPROX_EQUAL(monteCarloPrice, analyticalPrice, 0.01);
}



void testMargrabeOption() {
	TEST(testAnalyticalFormula);
}


