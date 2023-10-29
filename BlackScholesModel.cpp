#include "BlackScholesModel.h"

using namespace std;

#include "matlib.h"

BlackScholesModel::BlackScholesModel() :
    drift(0.0),
    stockPrice(0.0),
    volatility(0.0),
    riskFreeRate(0.0),
    date(0.0) {
}

/**
 *  Creates a price path according to the model parameters
 */
Matrix BlackScholesModel::
            generateRiskNeutralPricePaths(
        double toDate,
        int nPaths,
        int nSteps ) const {
    return generatePricePaths( toDate, nPaths, nSteps, riskFreeRate );
}

/**
 *  Creates a price path according to the model parameters
 */
Matrix BlackScholesModel::generatePricePaths(
        double toDate,
        int nPaths,
        int nSteps ) const {
    return generatePricePaths( toDate, nPaths, nSteps, drift );
}



/**
 *  Creates a price path according to the model parameters
 */
Matrix BlackScholesModel::generatePricePaths(
        double toDate,
        int nPaths,
        int nSteps,
        double drift ) const {
    Matrix path(nPaths, nSteps,0);            
    double dt = (toDate-date)/nSteps;
    double a = (drift - volatility*volatility*0.5)*dt;
    double b = volatility*sqrt(dt);
    Matrix currentLogS=log(stockPrice)*ones(nPaths,1);
    for (int i=0; i<nSteps; i++) {
        Matrix vals = randn( nPaths,1 ); 
		// vals contains epsilon
        vals*=b;   
        vals+=a;   // vals now contains dLogS
        vals+=currentLogS; // vals now contains logS
        currentLogS = vals;
        vals.exp(); // vals now contains S
        path.setCol( i, vals, 0 );
    }
    return path;
}
////////////////////////////////
//
//   TESTS
//
////////////////////////////////

void testRiskNeutralPricePath() {
    rng("default");

    BlackScholesModel bsm;
    bsm.riskFreeRate = 0.05;
    bsm.volatility = 0.1;
    bsm.stockPrice = 100.0;
    bsm.date = 2.0;

    int nPaths = 10000;
    int nsteps = 5;
    double maturity = 4.0;
    Matrix paths = 
        bsm.generateRiskNeutralPricePaths( maturity,
                                            nPaths,
                                            nsteps );
    Matrix finalPrices = paths.col( nsteps-1 );
    ASSERT_APPROX_EQUAL( meanCols( finalPrices ).asScalar(), 
        exp( bsm.riskFreeRate*2.0)*bsm.stockPrice, 0.5);
}

void testVisually() {
    BlackScholesModel bsm;
    bsm.riskFreeRate = 0.05;
    bsm.volatility = 0.1;
    bsm.stockPrice = 100.0;
    bsm.date = 2.0;

    int nSteps = 1000;
    double maturity = 4.0;

    Matrix path = bsm.generatePricePaths( maturity,
                                         1,
                                         nSteps );
    double dt = (maturity-bsm.date)/nSteps;
    Matrix times = linspace(dt, maturity, nSteps, 1 );
    plot("examplePricePath.html",
         times,
         path );
}


void testBlackScholesModel() {
    TEST( testRiskNeutralPricePath );
    TEST( testVisually );
}

