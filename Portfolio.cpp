#include "Portfolio.h"
#include "CallOption.h"
#include "PutOption.h"
#include "UpAndOutOption.h"
#include "MonteCarloPricer.h"

using namespace std;

/*  
 *  By using an abstract interface class with a factory constructor
 *  and only having the implementation in the C++ file we increase information
 *  hiding. Nobody knows about the PortfolioImpl class outside of the C++ file,
 *  so we can change it without any impact on anything else.
 */
class PortfolioImpl : public Portfolio {
public:
    /*  Returns the number of items in the portflio */
    int size() const;    
    /*  Add a new security to the portfolio, returns the index
        at which it was added */
    int add( double quantity,
		shared_ptr<ContinuousTimeOption> security);
    /*  Update the quantity at a given index */
    void setQuantity( int index, double quantity );
    /*  Compute the current price */
    double price( const MultiStockModel& model ) const;    

	/*  Price this portfolio using one consistent set of monte carlo simulations */
	double monteCarloPrice(
		const MultiStockModel& model, const MonteCarloPricer& pricer) const;
//private:
    vector<double> quantities;
	vector< shared_ptr<ContinuousTimeOption> > securities;
};

int PortfolioImpl::size() const {
    return quantities.size();
}

int PortfolioImpl::add( double quantity,
	shared_ptr<ContinuousTimeOption> security) {
    quantities.push_back( quantity );
    securities.push_back( security );
    return quantities.size();
}

double PortfolioImpl::price(
        const MultiStockModel& model ) const {
    double ret = 0;
    int n = size();
    for (int i=0; i<n; i++) {
        ret += quantities[i] * securities[i]->price( model );
    }
    return ret;
}

void PortfolioImpl::setQuantity( int index,
        double quantity ) {
    quantities[index] = quantity;
}

//
//  More efficient monte carlo pricing implementation
//  

/*  A Maturity Grouping is a sub portfolio consisting of
	securities with the same maturity */
class MaturityGrouping : public ContinuousTimeOption {
public:

	MaturityGrouping(double maturity) : maturity(maturity) {
	}

	double getMaturity() const {
		return maturity;
	}

	bool isPathDependent() const {
		for (auto& sec : securities) {
			if (sec->isPathDependent()) {
				return true;
			}
		}
		return false;
	}

	set<string> getStocks() const {
		set<string> ret;
		for (auto& sec : securities) {
			for (auto& stock : sec->getStocks()) {
				ret.insert(stock);
			}
		}
		return ret;
	}

	double price(const MultiStockModel& m) const {
		MonteCarloPricer pricer;
		return pricer.price(*this, m);
	}

	Matrix payoff(const MarketSimulation& simulation) const {
		ASSERT(securities.size() > 0);
		Matrix current = quantities[0]*securities[0]->payoff(simulation);
		for (int i = 1; i < (int)securities.size(); i++) {
			current += quantities[i] * securities[i]->payoff(simulation);
		}
		return current;
	}

	void add( double quantity, SPContinuousTimeOption o ) {
		quantities.push_back(quantity);
		securities.push_back(o);
	}
private:
	vector<SPContinuousTimeOption> securities;
	vector<double> quantities;
	double maturity;

};

typedef shared_ptr<MaturityGrouping> SPMaturityGrouping;

/*  Price this portfolio using one consistent set of monte carlo simulations */
double PortfolioImpl::monteCarloPrice(
	const MultiStockModel& model, const MonteCarloPricer& pricer) const {
	map<double, SPMaturityGrouping> maturityGroupings;
	for (int i = 0; i < (int)securities.size(); i++) {
		auto security = securities[i];
		double quantity = quantities[i];
		double maturity = security->getMaturity();
		auto pairPtr = maturityGroupings.find(maturity);
		if (pairPtr == maturityGroupings.end()) {
			SPMaturityGrouping newGrouping(new MaturityGrouping(maturity));
			maturityGroupings[maturity] = newGrouping;
			pairPtr = maturityGroupings.find(maturity);
		}
		pairPtr->second->add(quantity, security);
	}

	double ret = 0;
	for (auto& pair : maturityGroupings) {
		MaturityGrouping& grouping = *pair.second;
		ret += pricer.price( grouping, model );
	}
	return ret;
}


/**
 *   Create a Portfolio
 */
shared_ptr<Portfolio> Portfolio::newInstance() {
    shared_ptr<Portfolio> ret=make_shared<PortfolioImpl>();
    return ret;
}




/////////////////////////////
//  Tests
/////////////////////////////

static void testSingleSecurity() {
    shared_ptr<Portfolio> portfolio = Portfolio::newInstance();
    
    shared_ptr<CallOption> c=make_shared<CallOption>();
    c->setStrike(110);
    c->setMaturity(1.0);

    portfolio->add( 100, c );

    BlackScholesModel bsm;
    bsm.volatility = 0.1;
    bsm.stockPrice = 100;

	MultiStockModel msm(bsm);
    
    double unitPrice = c->price( msm );
    double portfolioPrice = portfolio->price( msm );
    ASSERT_APPROX_EQUAL( 100*unitPrice, portfolioPrice, 0.0001);
}

static void testPutCallParity() {
    shared_ptr<Portfolio> portfolio
        = Portfolio::newInstance();
    
    shared_ptr<CallOption> c=make_shared<CallOption>();
    c->setStrike(110);
    c->setMaturity(1.0);

    shared_ptr<PutOption> p=make_shared<PutOption>();
    p->setStrike(110);
    p->setMaturity(1.0);


    portfolio->add( 100, c );
    portfolio->add( -100, p );

    BlackScholesModel bsm;
    bsm.volatility = 0.1;
    bsm.stockPrice = 100;
    bsm.riskFreeRate = 0;

	MultiStockModel msm(bsm);

    double expected = bsm.stockPrice - c->getStrike();
    double portfolioPrice = portfolio->price( msm );
    
    ASSERT_APPROX_EQUAL(100*expected,portfolioPrice,0.0001);

}

void testMultiStockPortfolio() {
	auto model = MultiStockModel::createTestModel();
	auto p = Portfolio::newInstance();
	auto stocks = model.getStocks();

	double q0 = 1.0;
	auto stock0 = stocks[0];
	SPUpAndOutOption o0=make_shared<UpAndOutOption>();
	o0->setStock(stock0);
	o0->setStrike(model.getStockPrice(stock0));
	o0->setBarrier(2*model.getStockPrice(stock0));
	p->add(q0,o0);
	
	double q1 = 2.0;
	auto stock1 = stocks[1];
	SPUpAndOutOption o1=make_shared<UpAndOutOption>();
	o1->setStock(stock1);
	o1->setStrike(model.getStockPrice(stock1));
	o1->setBarrier(2 * model.getStockPrice(stock1));
	p->add(q1, o1);

	MonteCarloPricer pricer;
	pricer.nScenarios = 100000;
	double p0 = pricer.price(*o0,model);
	double p1 = pricer.price(*o1, model);
	double expected = q0*p0 + q1*p1;

	double actual = p->price(model);
	ASSERT_APPROX_EQUAL(expected, actual, 0.3);

	double calculatedDifferently = p->monteCarloPrice(model, pricer);
	ASSERT_APPROX_EQUAL(calculatedDifferently, actual, 0.3);
}

void testPerformanceImprovement() {
	shared_ptr<Portfolio> portfolio
		= Portfolio::newInstance();

	BlackScholesModel bsm;
	bsm.volatility = 0.1;
	bsm.stockPrice = 100;
	bsm.riskFreeRate = 0;

	for (int i = 0; i < 5; i++) {
		SPUpAndOutOption option(new UpAndOutOption());
		option->setBarrier(bsm.stockPrice + i / 100.0);
		option->setStrike(bsm.stockPrice);
		option->setMaturity(1.0);
		portfolio->add(1.0, option);
	}

	MultiStockModel msm(bsm);

	auto start1 = clock();
	portfolio->price(msm);
	auto diff1 = clock() - start1;

	auto start2 = clock();
	MonteCarloPricer pricer;
	portfolio->monteCarloPrice(msm, pricer);
	auto diff2 = clock() - start2;

	auto start3 = clock();
	pricer.nTasks = 10;
	portfolio->monteCarloPrice(msm, pricer);
	auto diff3 = clock() - start3;


	INFO("Naive method took " << diff1);
	INFO("Improved method took " << diff2);
	INFO("Multi threaded method took " << diff3);

}

void testPortfolio() {
    TEST( testSingleSecurity );
    TEST( testPutCallParity );
	TEST( testMultiStockPortfolio );
	TEST(testPerformanceImprovement);
}

