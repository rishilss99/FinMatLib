#pragma once

#include "stdafx.h"
#include "Matrix.h"

class MarketSimulation {
public:

	/**
	 *  Store a simulation
	 */
	void addSimulation(const std::string& stock,
		SPCMatrix matrix) {
		simulations[stock] = matrix;
	}

	/**
	 *   Returns a matrix of stock prices
	 *   rows represent different scenarios
	 *   columns represent different time points
	 */
	SPCMatrix getStockPrices( const std::string& stock)
		const {
		auto pos = simulations.find(stock);
		ASSERT(pos != simulations.end());
		return pos->second;
	}

private:
	std::map< std::string, SPCMatrix> simulations;
};