#pragma once

#include "stdafx.h"
#include "Matrix.h"

class BlackScholesModel {
public:
    BlackScholesModel();
    double drift;
    double stockPrice;
    double volatility;
    double riskFreeRate;
    double date;

    Matrix generatePricePaths(
                            double toDate,
                            int nPaths,
                            int nSteps) const;

    Matrix generateRiskNeutralPricePaths(
                            double toDate,
                            int nPaths,
                            int nSteps) const;

private:

    Matrix generatePricePaths(
                            double toDate,
                            int nPaths,
                            int nSteps,
                            double drift) const;
};



void testBlackScholesModel();