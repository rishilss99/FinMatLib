#pragma once

#include "stdafx.h"
#include "Matrix.h"
#include "BlackScholesModel.h"
#include "MarketSimulation.h"

/**
 *   A model for a collection of stocks that uses
 *   multi-dimensional Brownian motion
 */
class MultiStockModel {
public:
	/*  Create a model based on a 1-d black scholes model */
	explicit MultiStockModel(
		const BlackScholesModel& bsm );

	MultiStockModel(std::vector<std::string> stocks,
		Matrix stockPrices,
		Matrix drifts,
		Matrix covarianceMatrix);

	/*  The risk free rate */
	double getRiskFreeRate() const {
		return riskFreeRate;
	}
	/*  The current date in years */
	double getDate() const {
		return date;
	}
	/*  Setter */
	void setRiskFreeRate(double riskFreeRate) {
		this->riskFreeRate = riskFreeRate;
	}
	/*  Setter */
	void setDate(double date ) {
		this->date = date;
	}
	/*  Get the names of the stocks */
	std::vector<std::string> getStocks() {
		return stockNames;
	}

	double getStockPrice(const std::string& stock) {
		return stockPrices(getIndex(stock),0);
	}

	Matrix getCovarianceMatrix() {
		return covarianceMatrix;
	}

	double getCovarianceStockPair(const std::string& stock1, const std::string &stock2) const{
		int idx1 = getIndex(stock1);
		int idx2 = getIndex(stock2);
		return covarianceMatrix(idx1, idx2);
	}

	/*  Extract the 1-d sub model for a given
		stock code */
	BlackScholesModel getBlackScholesModel(
		const std::string& stockCode) const;

	/*  Get a sub model that uses only the given stocks */
	MultiStockModel getSubmodel(
		std::set<std::string> stocks) const;

	/*  Returns a simulation up to the given date
		in the P measure */
	MarketSimulation generatePricePaths(
		double toDate,
		int nPaths,
		int nSteps) const;
	/*  Returns a simulation up to the given date 
		in the Q measure */
	MarketSimulation generateRiskNeutralPricePaths(
		double toDate,
		int nPaths,
		int nSteps) const;
	/*  For testing it is useful to have a standard
		dummy name for stocks */
	static const std::string DEFAULT_STOCK;

	/*  Creates a standard 3d model for testing */
	static MultiStockModel createTestModel();
private:

	/*  Mapping from a stock code to the index
	    used in our matrices */
	std::unordered_map<std::string, int> stockToIndex;
	/*  The names of the stocks */
	std::vector<std::string> stockNames;
	/*  A column vector of drifts */
	Matrix drifts;
	/*  A column vector of current stock prices */
	Matrix stockPrices;
	/*  The covariance matrix */
	Matrix covarianceMatrix;
	/*  The risk free rate */
	double riskFreeRate;
	/*  The current date */
	double date;
	/*  Generate price paths with the given drifts */
	MarketSimulation generatePricePaths(
		double toDate,
		int nPaths,
		int nSteps,
		Matrix drifts) const;

	/*  Gets the index of a given stock in the matrices */
	int getIndex(const std::string&  stockCode)
			const {
		auto pos = stockToIndex.find(stockCode);
		ASSERT(pos != stockToIndex.end());
		int idx = pos->second;
		return idx;
	}
};




void testMultiStockModel();