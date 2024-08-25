#include "DataProcessorAndPlot.h"
#include "matlib.h"

using namespace std;

/**
 *  One if the most important classes I've written or kind of derived.
 *  What is does basically is unpacking a vector and pass the elements
 *  as a inputs to the function f. All done using templates.
 */
template <typename T, size_t num_args>
class UnpackCaller
{
private:
    template <typename FuncType, size_t... I>
    auto call(FuncType &f, vector<double> &args, index_sequence<I...>)
    {
        return f(args[I]...);
    }

public:
    template <typename FuncType>
    auto operator()(FuncType &f, vector<double> &args)
    {
        ASSERT(args.size() == num_args); // just to be sure
        return call(f, args, std::make_index_sequence<num_args>{});
    }
};

template <typename T>
DataProcessorAndPlot<T>::DataProcessorAndPlot(string &filePath) : filePath_(filePath)
{
    validateFilePath();
}

template <typename T>
void DataProcessorAndPlot<T>::validateFilePath() const
{
    if (filePath_.empty())
    {
        throw invalid_argument("Filepath is empty string");
    }
    if (filePath_.length() <= 4)
    {
        throw invalid_argument("File must be in .csv format");
    }
    if (filePath_.compare(filePath_.length() - 4, 4, ".csv"))
    {
        throw invalid_argument("File must be in .csv format");
    }
    if (!ifstream(filePath_).good())
    {
        throw invalid_argument("File invalid");
    }
}

template <typename T>
void DataProcessorAndPlot<T>::readData()
{
    ifstream fileData(filePath_);
    string line;
    while (getline(fileData, line))
    {
        istringstream ss(line);
        string token;

        vector<T> rowData;
        T temp;

        while (getline(ss, token, ','))
        {
            stringstream(token) >> temp;
            rowData.push_back(temp);
        }
        dataRowVec_.push_back(rowData);
    }
    ASSERT(!dataRowVec_.empty());
    fileData.close();
}

template <typename T>
template <typename F, typename U>
void DataProcessorAndPlot<T>::plotLineChart(F &func, U &unpackcaller, string plotTitle, string outputFile, size_t ignoreColIdx, size_t xAxisIdx)
{
    ASSERT((ignoreColIdx == SIZE_MAX) || (ignoreColIdx >= 0 && ignoreColIdx < dataRowVec_[0].size()));
    ASSERT((xAxisIdx >= 0 && xAxisIdx < dataRowVec_[0].size()));
    vector<T> xAxisData;
    vector<T> yAxisData;
    for (auto &rowData : dataRowVec_)
    {
        setDebugEnabled(true);
        DEBUG_PRINT(rowData[xAxisIdx]);
        setDebugEnabled(false);
        vector<T> tempVec{rowData};
        if (ignoreColIdx != SIZE_MAX)
        {
            tempVec.erase(tempVec.begin() + ignoreColIdx);
        }
        double res = unpackcaller(func, tempVec);
        xAxisData.push_back(rowData[xAxisIdx]);
        yAxisData.push_back(res);
    }
    lineChart_.setTitle(plotTitle);
    lineChart_.setSeries(xAxisData, yAxisData);
    lineChart_.writeAsHTML(outputFile);
}

// template <typename T>
// void DataProcessorAndPlot<T>::plotLineChart(function<T(initializer_list<T> list)> f, size_t xAxisIdx)
// {
//     for(auto &rowData: dataRowVec_)
//     {
//         initializer_list<T> ls = {rowData.begin(), rowData.end()-1};
//     }
// }

static void testReadData()
{
    string fileName = "test.csv";
    ofstream outFile(fileName);

    outFile << "5000,23.00,0.98"
            << "\n";
    outFile << "4000,45.00,0.35"
            << "\n";

    outFile.close();

    DataProcessorAndPlot<double> dp(fileName);

    dp.readData();

    vector<double> rowData0 = dp.getTopDataRow();

    remove(fileName.c_str());
    ASSERT_APPROX_EQUAL(rowData0[0], 5000, 0.1);
    ASSERT_APPROX_EQUAL(rowData0[1], 23.00, 0.1);
    ASSERT_APPROX_EQUAL(rowData0[2], 0.98, 0.1);
}

static void testplotLineChart()
{
    string fileName = "test.csv";
    ofstream outFile(fileName);

    outFile << "5000,23.00,0.98"
            << "\n";
    outFile << "4000,45.00,0.35"
            << "\n";
    outFile << "3000,43.00,0.67"
            << "\n";

    outFile.close();

    DataProcessorAndPlot<double> dp(fileName);

    dp.readData();
    vector<double> rowData0 = dp.getTopDataRow();

    remove(fileName.c_str());
    function<double(double, double)> f = [](double a, double b)
    { return a + b; };
    UnpackCaller<double, 2> up;
    dp.plotLineChart(f, up, "Test plot", "plotTest.html", 2);
}

static void testTwoYearImpliedVolatilityPlot()
{
    string fileName = "Year2Data.csv";
    DataProcessorAndPlot<double> dp(fileName);
    dp.readData();

    vector<double> rowData0 = dp.getTopDataRow();

    ASSERT_APPROX_EQUAL(rowData0[0], 3500, 0.1);
    ASSERT_APPROX_EQUAL(rowData0[1], 1544.3, 0.1);
    ASSERT_APPROX_EQUAL(rowData0[2], 0.3292, 0.1);

    double stockPrice = 4768.0;
    double r = 0.0380;
    double T = 2.0;
    double tolerance = 10.0;

    function<double(double, double)> impliedVolatilityFunc = [=](double strikePrice, double callOptionPrice)
    { return impliedVolatility(stockPrice, r, strikePrice, T, callOptionPrice, tolerance); };

    UnpackCaller<double, 2> up;

    dp.plotLineChart(impliedVolatilityFunc, up, "2 Year Call Option Plot", "plotImpliedVolatility.html", 2, 0);
}

void testDataProcessorAndPlot()
{
    TEST(testReadData);
    TEST(testplotLineChart);
    TEST(testTwoYearImpliedVolatilityPlot);
}