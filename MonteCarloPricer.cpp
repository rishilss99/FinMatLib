#include "MonteCarloPricer.h"

#include "matlib.h"
#include "CallOption.h"

using namespace std;


MonteCarloPricer::MonteCarloPricer() :
    nScenarios(10000),
    nSteps(10) {
}

double MonteCarloPricer::price(
	const ContinuousTimeOption& option,
	const BlackScholesModel& model) const {
	auto stocks = option.getStocks();
	assert(stocks.size() == 1);
	MultiStockModel msm(model);
	return price(option, msm);
}

double MonteCarloPricer::price(
        const ContinuousTimeOption& option,
        const MultiStockModel& model ) const {
    int nSteps = this->nSteps;
    if (!option.isPathDependent()) {
        nSteps = 1;
    }
    double total = 0.0;

	MultiStockModel subModel = model.getSubmodel(
		option.getStocks());
    
    // We price at most one million scenarios at a time to avoid running out of memory
    int batchSize = 1000000/nSteps;
    if (batchSize<=0) {
        batchSize = 1;
    }

    int scenariosRemaining = nScenarios;
    while (scenariosRemaining>0) {
        
        int thisBatch = batchSize;
        if (scenariosRemaining<batchSize) {
            thisBatch = scenariosRemaining;
        }

        MarketSimulation sim = subModel.
            generateRiskNeutralPricePaths(
                option.getMaturity(),
                thisBatch,
                nSteps );
        Matrix payoffs = option.payoff( sim );
        total+= sumCols( payoffs ).asScalar();
        scenariosRemaining-=thisBatch;
    }
    double mean = total/nScenarios;
    double r = model.getRiskFreeRate();
    double T = option.getMaturity() - model.getDate();
    return exp(-r*T)*mean;
}

//////////////////////////////////////
//
//   Tests
//
//////////////////////////////////////

static void testPriceCallOption() {
    rng("default");

    CallOption c;
    c.setStrike( 110 );
    c.setMaturity( 2 );

    BlackScholesModel m;
    m.volatility = 0.1;
    m.riskFreeRate = 0.05;
    m.stockPrice = 100.0;
    m.drift = 0.1;
    m.date = 1;

	MultiStockModel msm(m);

    MonteCarloPricer pricer;
    double price = pricer.price( c, m );
    double expected = c.price( msm );
    ASSERT_APPROX_EQUAL( price, expected, 0.1 );
}

void testMonteCarloPricer() {
    TEST( testPriceCallOption );
}