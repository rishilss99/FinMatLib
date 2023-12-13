#pragma once

#include "stdafx.h"
#include "Matrix.h"

class MarketSimulation {
public:

	/**
	 *  Store a simulation
	 */
	void addStockPaths(const std::string& stock,
		SPCMatrix matrix) {
		stockPaths[stock] = matrix;
	}

	/**
	 *   Returns a matrix of stock prices
	 *   rows represent different scenarios
	 *   columns represent different time points
	 */
	SPCMatrix getStockPaths( const std::string& stock)
		const {
		auto pos = stockPaths.find(stock);
		ASSERT(pos != stockPaths.end());
		return pos->second;
	}

private:
	std::unordered_map< std::string, SPCMatrix> stockPaths;
};