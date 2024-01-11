#include "MonteCarloPricer.h"

#include "matlib.h"
#include "CallOption.h"
#include "Executor.h"

using namespace std;

MonteCarloPricer::MonteCarloPricer() : nScenarios(100000),
									   nSteps(10),
									   nTasks(1)
{
}

double MonteCarloPricer::price(
	const ContinuousTimeOption &option,
	const BlackScholesModel &model) const
{
	auto stocks = option.getStocks();
	assert(stocks.size() == 1);
	MultiStockModel msm(model);
	return price(option, msm);
}

double singleThreadedPrice(
	int taskNumber,
	int nScenarios,
	int nSteps,
	const ContinuousTimeOption &option,
	const MultiStockModel &model)
{

	if (!option.isPathDependent())
	{
		nSteps = 1;
	}
	double total = 0.0;

	MultiStockModel subModel = model.getSubmodel(
		option.getStocks());

	long long randSize = subModel.randSize(nScenarios,
										   nSteps);
	mt19937 rng;
	rng.discard(randSize * taskNumber);

	// We price at most one million scenarios at a time to avoid running out of memory
	int batchSize = 1000000 / nSteps;
	if (batchSize <= 0)
	{
		batchSize = 1;
	}

	int scenariosRemaining = nScenarios;
	while (scenariosRemaining > 0)
	{

		int thisBatch = batchSize;
		if (scenariosRemaining < batchSize)
		{
			thisBatch = scenariosRemaining;
		}

		MarketSimulation sim = subModel.generateRiskNeutralPricePaths(
			rng,
			option.getMaturity(),
			thisBatch,
			nSteps);
		Matrix payoffs = option.payoff(sim);
		total += sumCols(payoffs).asScalar();
		scenariosRemaining -= thisBatch;
	}
	double mean = total / nScenarios;
	double r = model.getRiskFreeRate();
	double T = option.getMaturity() - model.getDate();
	return exp(-r * T) * mean;
}

class PriceTask : public Task
{
public:
	/*  Amount of random numbers to skip */
	int taskNumber;
	int nScenarios, nSteps;
	const ContinuousTimeOption &option;
	const MultiStockModel &model;
	/*  Output data */
	double result;

	PriceTask(
		int taskNumber,
		int nScenarios,
		int nSteps,
		const ContinuousTimeOption &option,
		const MultiStockModel &model)
		: taskNumber(taskNumber),
		  nScenarios(nScenarios),
		  nSteps(nSteps),
		  option(option),
		  model(model)
	{
	}

	void execute()
	{
		result = singleThreadedPrice(taskNumber,
									 nScenarios, nSteps, option, model);
	}
};

/**
 *   Price the option by Monte Carlo
 */
// double MonteCarloPricer::price(
// 	const ContinuousTimeOption& option,
// 	const MultiStockModel& model) const {
// 	ASSERT(nTasks >= 1);
// 	vector< shared_ptr<PriceTask> > tasks;
// 	shared_ptr<Executor> executor =
// 		Executor::newInstance(nTasks);
// 	for (int i = 0; i<nTasks; i++) {
// 		shared_ptr<PriceTask> task(new PriceTask(
// 			i, nScenarios/nTasks,
// 			nSteps, option, model));
// 		tasks.push_back(task);
// 		executor->addTask(task);
// 	}
// 	executor->join();
// 	double total = 0.0;
// 	for (int i = 0; i<nTasks; i++) {
// 		total += tasks[i]->result;
// 	}
// 	return total / nTasks;
// }

/**
 *   Price the option by Monte Carlo
 */
double MonteCarloPricer::price(
	const ContinuousTimeOption &option,
	const MultiStockModel &model) const
{
	ASSERT(nTasks >= 1);
	vector<double> prices(nTasks, 0.0);
	shared_ptr<Executor> executor =
		Executor::newInstance(nTasks);
	for (int i = 0; i < nTasks; i++)
	{
		auto lmbd = [this, i, &prices, &option, &model]()
		{ prices[i] = singleThreadedPrice(i, this->nScenarios / this->nTasks, this->nSteps, option, model); };
		executor->addTask(lmbd);
	}
	executor->join();
	double total = accumulate(prices.begin(), prices.end(), 0.0);
	return total / nTasks;
}

//////////////////////////////////////
//
//   Tests
//
//////////////////////////////////////

static void testPriceCallOption()
{
	rng("default");

	CallOption c;
	c.setStrike(110);
	c.setMaturity(2);

	BlackScholesModel m;
	m.volatility = 0.1;
	m.riskFreeRate = 0.05;
	m.stockPrice = 100.0;
	m.drift = 0.1;
	m.date = 1;

	MultiStockModel msm(m);

	MonteCarloPricer pricer;
	pricer.nTasks = 1;
	double price = pricer.price(c, m);
	double expected = c.price(msm);
	pricer.nTasks = 10;
	double price2 = pricer.price(c, m);
	ASSERT_APPROX_EQUAL(price, expected, 0.1);
	ASSERT_APPROX_EQUAL(price2, price, 0.000001);
}

void testMonteCarloPricer()
{
	TEST(testPriceCallOption);
}