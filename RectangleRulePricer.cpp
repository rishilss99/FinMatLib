#include "RectangleRulePricer.h"
#include "matlib.h"
#include "CallOption.h"
#include "geometry.h"

using namespace std;

/*
 *  C++11 contains an isfinite function already, but we have
 *  written our own so the code can work on older versions of
 *  Visual Studio
 */
bool isfinitenumber(double arg)
{
    return arg == arg &&
           arg != std::numeric_limits<double>::infinity() &&
           arg != -std::numeric_limits<double>::infinity();
}

double RectangleRulePricer::price(const PathIndependentOption &option,
                                  const BlackScholesModel &model) const
{
    return price([&](double S)
                 { return option.payoff((Matrix)S)(0); },
                 option.getMaturity(), model);
}

double RectangleRulePricer::price(std::function<double(double)> payoffFunction,
                                  double maturity,
                                  const BlackScholesModel &model) const
{
    auto func = [&](double S)
    {
        double sigma = model.volatility;
        double muTilde = model.riskFreeRate - 0.5 * sigma * sigma;
        double S0 = model.stockPrice;
        double T = maturity - model.date;

        double probDensity = (1 / (sigma * sqrt(2 * PI * T))) * exp(-pow(S - log(S0) - muTilde * T, 2) / (2 * sigma * sigma * T));
        double payoff = payoffFunction(exp(S));
        if (isfinitenumber(payoff))
        {
            return payoff * probDensity;
        }
        else
        {
            // payoffs that can't be evaluated, given
            // the computer's precision are taken to be zero.
            return 0.0;
        }
    };

    double expectation = integralOverR(func, nSteps);
    return exp(-model.riskFreeRate * (maturity - model.date)) * expectation;
}

static void testRectangleRuleCallOption()
{
    BlackScholesModel m;
    m.stockPrice = 100.0;
    m.volatility = 0.1;
    m.drift = 0.0;
    m.riskFreeRate = 0.1;
    m.date = 1;
    CallOption c;
    c.setStrike(100.0);
    c.setMaturity(2.0);
    double expected = c.price(MultiStockModel(m));
    RectangleRulePricer pricer;
    double actual = pricer.price(c, m);
    ASSERT_APPROX_EQUAL(actual, expected, 0.01);
}

void testRectangleRulePricer()
{
    TEST(testRectangleRuleCallOption);
}