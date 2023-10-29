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
        const BlackScholesModel& model ) {
    int nSteps = this->nSteps;
    if (!option.isPathDependent()) {
        nSteps = 1;
    }
    double total = 0.0;
    
    // We price at most one million scenarios at a time to avoid running out of memory
    int batchSize = 10000000/nSteps;
    if (batchSize<=0) {
        batchSize = 1;
    }

    int scenariosRemaining = nScenarios;
    while (scenariosRemaining>0) {
        
        int thisBatch = batchSize;
        if (scenariosRemaining<batchSize) {
            thisBatch = scenariosRemaining;
        }

        Matrix paths= model.
            generateRiskNeutralPricePaths(
                option.getMaturity(),
                thisBatch,
                nSteps );
        Matrix payoffs = option.payoff( paths );
        total+= sumCols( payoffs ).asScalar();
        scenariosRemaining-=thisBatch;
    }
    double mean = total/nScenarios;
    double r = model.riskFreeRate;
    double T = option.getMaturity() - model.date;
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

    MonteCarloPricer pricer;
    double price = pricer.price( c, m );
    double expected = c.price( m );
    ASSERT_APPROX_EQUAL( price, expected, 0.1 );
}

void testMonteCarloPricer() {
    TEST( testPriceCallOption );
}