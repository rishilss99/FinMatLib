#include "RectangleRulePricer.h"

#include "RealFunction.h"
#include "matlib.h"
#include "geometry.h"
#include "CallOption.h"
#include <cmath>

using namespace std;

/*
*  C++11 contains an isfinite function already, but we have
*  written our own so the code can work on older versions of
*  Visual Studio
*/
bool isfinitenumber(double arg) {
	return arg == arg &&
		arg != std::numeric_limits<double>::infinity() &&
		arg != -std::numeric_limits<double>::infinity();
}

double RectangleRulePricer::price(const PathIndependentOption& option,
	const BlackScholesModel& model) const {
	return price([&] (double stockPrice) {
			return option.payoffAtMaturity( Matrix(stockPrice) )(0);
		},
		option.getMaturity(),
		model);
}

double RectangleRulePricer::price(function<double(double)> payoffFunction,
	double maturity,
	const BlackScholesModel& model) const {

	auto integrand = [&]( double x) {
		double sigma = model.volatility;
		double muTilde = model.riskFreeRate - 0.5*sigma*sigma;
		double S0 = model.stockPrice;
		double t = maturity;

		double probDensity = 1 / (sigma*sqrt(2 * PI*t)) * exp(-pow(x - log(S0) - muTilde*t, 2) / (2 * sigma*sigma*t));
		double payoff = payoffFunction(exp(x));
		if (isfinitenumber(payoff)) {
			return payoff * probDensity;
		}
		else {
			// payoffs that can't be evaluated, given
			// the computer's precision are taken to be zero.
			return 0.0;
		}
		return 0.0;
	};

	double expectation = integralOverR(integrand, 1000);
	return exp(-model.riskFreeRate*maturity)*expectation;

}

static void testPriceCallOption() {
	BlackScholesModel m;
	m.stockPrice = 100.0;
	m.volatility = 0.1;
	m.drift = 0.0;
	m.riskFreeRate = 0.1;
	CallOption c;
	c.setStrike(100.0);
	c.setMaturity(2.0);
	double expected = c.price(MultiStockModel(m));
	RectangleRulePricer pricer;
	double actual = pricer.price(c, m);
	ASSERT_APPROX_EQUAL(actual, expected, 0.01);
}


void testRectangleRulePricer() {
	TEST(testPriceCallOption);
}