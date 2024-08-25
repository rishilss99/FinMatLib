#include "UpAndOutOption.h"
#include "KnockoutOption.h"
#include "MonteCarloPricer.h"
#include "matlib.h"

using namespace std;

Matrix UpAndOutOption::payoff(
        const Matrix& prices ) const {
    Matrix max = maxOverRows( prices );
    Matrix didntHit = max < getBarrier();
    Matrix p = prices.col( prices.nCols()-1);
    p -= getStrike();
    p.positivePart();
    p.times(didntHit);
    return p;
}

/////////////////////////////////////
//
//   TESTS
//
/////////////////////////////////////

static void testPayoff() {
    UpAndOutOption o;
    o.setBarrier(100);
    o.setStrike(70);
    Matrix prices(1,2);
    prices(0,0)=120;
    prices(0,1)=80;
    ASSERT_APPROX_EQUAL( o.payoff( prices ).asScalar(), 0.0, 0.001);
    prices(0,0) = 90;
    ASSERT_APPROX_EQUAL( o.payoff( prices ).asScalar(), 10.0, 0.001);
    prices(0,1) = 60;
    ASSERT_APPROX_EQUAL( o.payoff( prices ).asScalar(), 0.0, 0.001);
}

static void testPerformance() {
    BlackScholesModel model;
    model.stockPrice = 100;
    model.volatility = 0.1;
    
    UpAndOutOption o;
    o.setBarrier(120);
    o.setStrike(110);
    o.setMaturity(1.0);
    
    MonteCarloPricer pricer;
    clock_t start = clock();
    pricer.nScenarios = 1000;
    pricer.nSteps = 365;

    double price = pricer.price( o, model );
    double elapsed = (double)(clock()-start);
    std::cout<< "Price "<<price<<"\n";
    std::cout<< "Pricing took "<<(elapsed/CLOCKS_PER_SEC)<<"s\n";
}


void testUpAndOutOption() {
    TEST( testPayoff );
    TEST( testPerformance );
}