#include "include/matlib.h"
#include "include/geometry.h"
#include "include/textfunctions.h"
#include "include/CallOption.h"
#include "include/PutOption.h"
#include "include/PieChart.h"
#include "include/LineChart.h"
#include "include/BlackScholesModel.h"
#include "include/MultiStockModel.h"
#include "include/Histogram.h"
#include "include/MonteCarloPricer.h"
#include "include/UpAndOutOption.h"
#include "include/DownAndOutOption.h"
#include "include/Portfolio.h"
#include "include/Matrix.h"
#include "include/Executor.h"
#include "include/threadingexamples.h"
#include "include/MargrabeOption.h"
#include "include/RectangleRulePricer.h"

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
	testExecutor();
	testThreadingExamples();
	testUpAndOutOption();
	testMargrabeOption();
	testRectangleRulePricer();
    return 0;
}