#include "MultiStockModel.h"

using namespace std;

#include "matlib.h"

/*  The default name of a stock when non is provided */
string const MultiStockModel::DEFAULT_STOCK = "Acme";

MultiStockModel::MultiStockModel(
	const BlackScholesModel& bsm) {

	int nStocks = 1;
	stockCodeToIndex[DEFAULT_STOCK] = 0;
	stockNames.push_back(DEFAULT_STOCK);

	drifts = Matrix(nStocks, 1);
	drifts(0) = bsm.drift;
	covarianceMatrix = Matrix(nStocks, 1);
	covarianceMatrix(0, 0) = bsm.volatility*bsm.volatility;
	stockPrices = Matrix(nStocks, 1);
	stockPrices(0) = bsm.stockPrice;
	riskFreeRate = bsm.riskFreeRate;
	date = bsm.date;
}


MultiStockModel::MultiStockModel(std::vector<std::string> stocks,
		Matrix stockPrices,
		Matrix drifts,
		Matrix covarianceMatrix) : riskFreeRate(1.0), date(0.0) {
	int n = stocks.size();
	ASSERT(stockPrices.nRows() == n);
	ASSERT(stockPrices.nCols() == 1);
	ASSERT(drifts.nRows() == n);
	ASSERT(drifts.nCols() == 1);
	ASSERT(covarianceMatrix.nRows() == n);
	ASSERT(covarianceMatrix.nCols() == n);
	this->stockNames = stocks;
	this->stockPrices = stockPrices;
	this->drifts = drifts;
	this->covarianceMatrix = covarianceMatrix;
	int i = 0;
	for (auto& s : stocks) {
		stockCodeToIndex[s] = i++;
	}
}

/*  Get a sub model that uses only the given stocks */
MultiStockModel MultiStockModel::getSubmodel(
	set<string> stocks) const {

	int n = stocks.size();
	Matrix drifts(n, 1);
	Matrix stockPrices(n, 1);
	vector<string> newStocks(stocks.begin(), stocks.end());
	Matrix cov(n, n);

	int newIndex = 0;
	for (auto& stock : stocks) {
		int idx = getIndex(stock);
		drifts(newIndex) = this->drifts(idx);
		stockPrices(newIndex) = this->stockPrices(idx);
		newIndex++;
	}

	int i = 0;
	for (auto& stockI : stocks) {
		int j = 0;
		for (auto& stockJ : stocks) {
			int oldI = getIndex(stockI);
			int oldJ = getIndex(stockJ);
			cov(i, j) = covarianceMatrix(oldI, oldJ);
			j++;
		}
		i++;
	}
	MultiStockModel ret(newStocks, stockPrices, drifts, cov);
	ret.setDate(getDate());
	ret.setRiskFreeRate(getRiskFreeRate());
	return ret;
}

/*  Extracts a 1-d sub model */
BlackScholesModel MultiStockModel::getBlackScholesModel(
		const std::string& stockCode) const {
	int idx = getIndex(stockCode);
	BlackScholesModel bsm;
	bsm.drift = drifts(idx, 0);
	bsm.volatility = sqrt(covarianceMatrix(idx, idx));
	bsm.riskFreeRate = riskFreeRate;
	bsm.stockPrice = stockPrices(idx, 0);
	bsm.date = date;
	return bsm;
}


/*  Returns a simulation up to the given date
in the P measure */
MarketSimulation MultiStockModel::generatePricePaths(
	mt19937& rng,
	double toDate,
	int nPaths,
	int nSteps) const {
	return generatePricePaths(rng, toDate,nPaths,nSteps,drifts);
}

/*  Returns a simulation up to the given date
in the Q measure */
MarketSimulation MultiStockModel::generateRiskNeutralPricePaths(
	mt19937& rng,
	double toDate,
	int nPaths,
	int nSteps) const {
	Matrix riskNeutralDrifts = ones(drifts.nRows(), 1)*riskFreeRate;
	return generatePricePaths(rng, toDate, nPaths, nSteps, riskNeutralDrifts);
}


/**
*  Creates a price path according to the model parameters
*/
MarketSimulation MultiStockModel::generatePricePaths(
	mt19937& rng,
	double toDate,
	int nPaths,
	int nSteps,
	Matrix drifts) const {

	int nStocks = stockPrices.nRows();
	double dt = (toDate - date) / nSteps;
	double rootDt = sqrt(dt);

	// initialize matrices of simulations for 
	// each stock
	std::vector< SPMatrix> simulations;
	for (int j = 0; j < nStocks; j++) {
		SPMatrix matrix(new Matrix(nPaths, nSteps));
		simulations.push_back(matrix);
	}

	Matrix A = chol(covarianceMatrix);

	
	// create a matrix containing current log stock prices
	// and a matrix contianing the drift term to add each
	// time step
	Matrix currentLogStock(nPaths, nStocks);
	Matrix driftTerm(nPaths, nStocks);
	Matrix oneV = ones(nPaths, 1);
	for (int j = 0; j < nStocks; j++) {
		double S0 = stockPrices(j);
		currentLogStock.setCol(j, oneV*log(S0), 0);
		double logDrift = drifts(j) - 0.5*covarianceMatrix(j,j);
		driftTerm.setCol(j, oneV*logDrift*dt, 0);
	}

	// comute paths at subsequent time steps
	for (int i = 0; i < nSteps; i++) {
		Matrix epsilons = randn(rng, nPaths, nStocks);
		Matrix W = rootDt * epsilons * transpose(A);
		currentLogStock += driftTerm + W;
		Matrix currentStock = exp( currentLogStock );
		for (int j = 0; j < nStocks; j++) {
			auto stockPaths = simulations[j];
			stockPaths->setCol(i, currentStock, j);
		}
	}

	// store the results in a Market Simulation
	MarketSimulation sim;
	for (int j = 0; j < nStocks; j++) {
		auto stockPaths = simulations[j];
		sim.addSimulation(stockNames[j], stockPaths);
	}
	return sim;
}

/*  
 *   Create a standard model for testing
 */
MultiStockModel MultiStockModel::createTestModel() {
	vector<string> stocks({ "Acme", "Bigbank", "Chumhum" });
	Matrix prices("100;200;300");
	Matrix drifts("0;0;0");
	Matrix cov("5,2,1;2,6,-1;1,-1,7");
	cov *= 0.01;
	MultiStockModel msm(stocks, prices, drifts, cov);
	return msm;
}

//
//    Tests
//  

static void testCorrectCovarianceMatrix() {
	rng("default");

	MultiStockModel msm = MultiStockModel::createTestModel();
	int nPaths = 100000;
	int nSteps = 5;
	mt19937 rng;
	MarketSimulation sim = msm.generatePricePaths(rng, 1.0, nPaths, nSteps);
	auto cov = msm.getCovarianceMatrix();

	Matrix x(nPaths, 1);
	Matrix y(nPaths, 1);

	auto stocks = msm.getStocks();
	for (int i = 0; i < (int)stocks.size(); i++) {
		SPCMatrix m = sim.getStockPrices(stocks[i]);	
		x.setCol(0, *m, nSteps - 1);
		x.log();
		for (int j = 0; j < (int)stocks.size(); j++) {
			SPCMatrix n = sim.getStockPrices(stocks[j]);
			y.setCol(0, *n, nSteps - 1);
			y.log();
			x -= meanCols(x)(0,0);
			y -= meanCols(y)(0, 0);
			double sumProd = sumCols(dotTimes(x, y))(0,0);
			double covXY = sumProd / nPaths;
			ASSERT_APPROX_EQUAL(cov(i, j), covXY, 0.001);
		}
	}
}

void testMultiStockModel() {
	// our tests of the BlackScholesModel perform a great deal
	// of testing of this class already. This is because
	// BlackScholesModel has been refactored to use a 
	// MultiStockModel to generate stock prices.
	testCorrectCovarianceMatrix();
}

