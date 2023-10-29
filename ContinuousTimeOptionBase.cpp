#include "ContinuousTimeOptionBase.h"
#include "MonteCarloPricer.h"
#include "UpAndOutOption.h"
#include "CallOption.h"
#include "matlib.h"
#include "testing.h"

using namespace std;

double ContinuousTimeOptionBase::price(const BlackScholesModel &model) const
{
    MonteCarloPricer pricer;
    return pricer.price(*this, model);
}

//////////////////////////////
//
//  Tests
//
//////////////////////////////

static void testPrice()
{
    rng("default");

    BlackScholesModel bsm;
    bsm.volatility = 0.1;
    bsm.stockPrice = 100;

    CallOption callOption;
    callOption.setStrike(100);
    callOption.setMaturity(1.0);

    UpAndOutOption knockoutOption;
    knockoutOption.setStrike(100);
    knockoutOption.setMaturity(1.0);
    knockoutOption.setBarrier(1000);

    ContinuousTimeOptionBase &o1 = callOption;
    ContinuousTimeOptionBase &o2 = knockoutOption;

    double p1 = o1.price(bsm);
    double p2 = o2.price(bsm);

    ASSERT_APPROX_EQUAL(p1, p2, 0.1);
}

static void testOptionDebugPrint()
{
    UpAndOutOption upo;
    upo.setStrike(100.0);
    upo.setMaturity(1.0);
    upo.setBarrier(110.0);
    cout << upo;

    CallOption co;
    co.setStrike(200.0);
    co.setMaturity(2.0);
    cout << co;
}

void testContinuousTimeOptionBase()
{
    TEST(testPrice);
    TEST(testOptionDebugPrint);
}