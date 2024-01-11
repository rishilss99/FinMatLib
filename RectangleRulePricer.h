#pragma once

#include "stdafx.h"
#include "PathIndependentOption.h"
#include "BlackScholesModel.h"


class RectangleRulePricer {
public:
	int nSteps;
	double price(const PathIndependentOption& option,
		const BlackScholesModel& model) const;

	double price(std::function<double(double)> payoffFunction,
				 double maturity,
				 const BlackScholesModel& model) const;

	RectangleRulePricer() : nSteps(1000) {}
};


void testRectangleRulePricer();