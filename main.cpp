#include "matlib.h"
#include "geometry.h"
#include "textfunctions.h"
#include "CallOption.h"
#include "PutOption.h"
#include "PieChart.h"
#include "LineChart.h"
#include "BlackScholesModel.h"
#include "MultiStockModel.h"
#include "Histogram.h"
#include "MonteCarloPricer.h"
#include "UpAndOutOption.h"
#include "DownAndOutOption.h"
#include "Portfolio.h"
#include "Matrix.h"
#include "HashMap.h"
#include "MargrabeOption.h"


using namespace std;

int main() {

    testMatrix();
    testMatlib();
    testMultiStockModel();
	testBlackScholesModel();
	testGeometry();
    testPieChart();
    testCallOption();
    testPutOption();
    testLineChart();
    testTextFunctions();
    testHistogram();
    testMonteCarloPricer();
    testDownAndOutOption();
    testContinuousTimeOptionBase();
    testPortfolio();
    testPutOption();
    testUpAndOutOption();
    testHashMap();
    testMargrabeOption();
    return 0;
}