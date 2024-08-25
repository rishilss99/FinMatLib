#include "DownAndOutOption.h"
#include "KnockoutOption.h"
#include "matlib.h"

using namespace std;

Matrix DownAndOutOption::payoff(
        const Matrix& prices ) const {
    Matrix min = minOverRows( prices );
    Matrix didntHit = min > getBarrier();
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


void testDownAndOutOption() {
    DownAndOutOption o;
    o.setBarrier(50);
    o.setStrike(70);
    Matrix prices(1,2);
    prices(0,0)=120;
    prices(0,1)=80;
    ASSERT_APPROX_EQUAL( o.payoff( prices ).asScalar(), 10.0, 0.001);
    prices(0,0) = 40;
    ASSERT_APPROX_EQUAL( o.payoff( prices ).asScalar(), 0.0, 0.001);
}