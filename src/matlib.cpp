#include "matlib.h"

#include "geometry.h"
#include "LineChart.h"
#include "Histogram.h"
#include "RealFunction.h"
#include "Executor.h"

extern template class LineChart<double>;

using namespace std;

/*  Create a linearly spaced vector */
Matrix linspace(double from, double to, int numPoints, bool rowVector)
{
    ASSERT(numPoints >= 2);
    int nrows, ncols;
    if (rowVector)
    {
        nrows = 1;
        ncols = numPoints;
    }
    else
    {
        nrows = numPoints;
        ncols = 1;
    }
    Matrix ret(nrows, ncols);
    double step = (to - from) / (numPoints - 1);
    double current = from;
    for (int i = 0; i < numPoints; i++)
    {
        ret(i) = current;
        current += step;
    }
    return ret;
}

/**
 *  Sum the rows of a matrix
 */
Matrix sumRows(const Matrix &m)
{
    int nrow = m.nRows();
    int ncol = m.nCols();
    Matrix ret(nrow, 1, 0);
    for (int row = 0; row < nrow; row++)
    {
        double total = 0.0;
        const double *start = m.begin() + m.offset(row, 0);
        for (int col = 0; col < ncol; col++)
        {
            total += *(start);
            start += nrow;
        }
        ret(row, 0) = total;
    }
    return ret;
}

/**
 *  Sum the cols of a matrix
 */
Matrix sumCols(const Matrix &m)
{
    int ncol = m.nCols();
    Matrix ret(1, ncol, 0);
    for (int col = 0; col < ncol; col++)
    {
        double total = 0.0;
        const double *start = m.begin() + m.offset(0, col);
        const double *end = start + m.nRows();
        while (start != end)
        {
            total += *(start++);
        }
        ret(0, col) = total;
    }
    return ret;
}

/**
 *  The mean of the rows of a matrix
 */
Matrix meanRows(const Matrix &m)
{
    Matrix ret = sumRows(m);
    ret *= (1.0 / m.nCols());
    return ret;
}

/**
 *  The mean of the cols of a matrix
 */
Matrix meanCols(const Matrix &m)
{
    Matrix ret = sumCols(m);
    ret *= (1.0 / m.nRows());
    return ret;
}

/*  Compute the standard deviation of a matrix's rows */
Matrix stdRows(const Matrix &m, bool population)
{
    int n = m.nCols();
    Matrix total(m.nRows(), 1);
    Matrix totalSq(m.nRows(), 1);
    for (int i = 0; i < n; i++)
    {
        Matrix val = m.col(i);
        total += val;
        val.times(val);
        totalSq += val;
    }
    Matrix val = total;
    val.times(val);
    val *= (-1.0) / n;
    val += totalSq;
    double factor = population ? (1.0) / n : (1.0) / (n - 1);
    val *= factor;
    val.sqrt();
    return val;
}

/*  Compute the standard deviation of a matrix's cols */
Matrix stdCols(const Matrix &m, bool population)
{
    int n = m.nRows();
    Matrix total(1, m.nCols(), 1);
    Matrix totalSq(1, m.nCols(), 1);
    for (int i = 0; i < n; i++)
    {
        Matrix val = m.row(i);
        total += val;
        val.times(val);
        totalSq += val;
    }
    Matrix val = total;
    val.times(val);
    val *= (-1.0) / n;
    val += totalSq;
    double factor = population ? (1.0) / n : (1.0) / (n - 1);
    val *= factor;
    val.sqrt();
    return val;
}

/**
 *   Find the minimum across the cols of a vector
 */
Matrix minOverCols(const Matrix &m)
{
    Matrix minRow = m.row(0);
    for (int i = 1; i < m.nRows(); i++)
    {
        Matrix currentRow = m.row(i);
        Matrix test = currentRow < minRow;
        test.test(currentRow, minRow);
        minRow = test;
    }
    return minRow;
}

/**
 *   Find the minimum across the cols of a vector
 */
Matrix maxOverCols(const Matrix &m)
{
    Matrix maxRow = m.row(0);
    for (int i = 1; i < m.nRows(); i++)
    {
        Matrix currentRow = m.row(i);
        Matrix test = currentRow > maxRow;
        test.test(currentRow, maxRow);
        maxRow = test;
    }
    return maxRow;
}

/**
 *   Find the minimum across the rows of a vector
 */
Matrix minOverRows(const Matrix &m)
{
    Matrix minCol = m.col(0);
    for (int i = 1; i < m.nCols(); i++)
    {
        Matrix currentCol = m.col(i);
        Matrix test = currentCol < minCol;
        test.test(currentCol, minCol);
        minCol = test;
    }
    return minCol;
}

/**
 *   Find the minimum across the rows of a vector
 */
Matrix maxOverRows(const Matrix &m)
{
    Matrix maxCol = m.col(0);
    for (int i = 1; i < m.nCols(); i++)
    {
        Matrix currentCol = m.col(i);
        Matrix test = currentCol > maxCol;
        test.test(currentCol, maxCol);
        ;
        maxCol = test;
    }
    return maxCol;
}

/*  MersenneTwister random number generator */
static mt19937 mersenneTwister;
/*  Mutex to protect static var */
static mutex rngMutex;

/*  Reset the random number generator.
We ignore the description string */
void rng(const string &description)
{
    ASSERT(description == "default");
    lock_guard<mutex> lock(rngMutex);
    mersenneTwister.seed(mt19937::default_seed);
}

/*  Generate random numbers */
Matrix randuniform(int rows, int cols)
{
    lock_guard<mutex> lock(rngMutex);
    return randuniform(mersenneTwister, rows, cols);
}

/*  Create uniformly distributed random numbers using
the Mersenne Twister algorithm. See the code above for the answer
to the homework excercise which should familiarize you with the C API*/
Matrix randuniform(mt19937 &random, int rows, int cols)
{
    Matrix ret(rows, cols, 0);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            ret(i, j) = (random() + 0.5) / (random.max() + 1.0);
        }
    }
    return ret;
}

/**
 *  Generate random numbers
 */
Matrix randn(int rows, int cols)
{
    lock_guard<mutex> lock(rngMutex);
    return randn(mersenneTwister, rows, cols);
}

/*  Create normally distributed random numbers */
Matrix randn(mt19937 &random, int rows, int cols)
{
    Matrix ret = randuniform(random, rows, cols);
    for (int j = 0; j < cols; j++)
    {
        for (int i = 0; i < rows; i++)
        {
            ret(i, j) = norminv(ret(i, j));
        }
    }
    return ret;
}
/**
 *  Sort the rows of a matrix
 */
Matrix sortRows(const Matrix &m)
{
    Matrix copy(m.nRows(), m.nCols(), 0);
    for (int i = 0; i < m.nRows(); i++)
    {
        vector<double> row = m.row(i).rowVector();
        std::sort(row.begin(), row.end());
        Matrix asMatrix(row, 1);
        copy.setRow(i, asMatrix, 0);
    }
    return copy;
}

/**
 *  Sort the rows of a matrix
 */
Matrix sortCols(const Matrix &m)
{
    Matrix copy(m.nRows(), m.nCols(), 0);
    for (int i = 0; i < m.nCols(); i++)
    {
        vector<double> col = m.col(i).colVector();
        std::sort(col.begin(), col.end());
        Matrix asMatrix(col);
        copy.setCol(i, asMatrix, 0);
    }
    return copy;
}

/**
 *  Find the given percentile of a distribution
 */
static double prctile(const std::vector<double> &in, double percentage)
{
    // See the MATLAB documentation for a specification of what prctile actually does
    // its a little fiddly. The tests were all computed using MATLAB
    ASSERT(percentage >= 0.0);
    ASSERT(percentage <= 100.0);
    int n = in.size();

    vector<double> sorted = in;
    std::sort(sorted.begin(), sorted.end());

    int indexBelow = (int)(n * percentage / 100.0 - 0.5);
    int indexAbove = indexBelow + 1;
    if (indexAbove > n - 1)
    {
        return sorted[n - 1];
    }
    if (indexBelow < 0)
    {
        return sorted[0];
    }
    double valueBelow = sorted[indexBelow];
    double valueAbove = sorted[indexAbove];
    double percentageBelow = 100.0 * (indexBelow + 0.5) / n;
    double percentageAbove = 100.0 * (indexAbove + 0.5) / n;
    if (percentage <= percentageBelow)
    {
        return valueBelow;
    }
    if (percentage >= percentageAbove)
    {
        return valueAbove;
    }
    double correction = (percentage - percentageBelow) * (valueAbove - valueBelow) / (percentageAbove - percentageBelow);
    return valueBelow + correction;
}

/**
 *   Return the given percentile on each row
 */
Matrix prctileRows(const Matrix &m, double percentage)
{
    Matrix ret(m.nRows(), 1, 0);
    for (int i = 0; i < m.nRows(); i++)
    {
        vector<double> row = m.row(i).rowVector();
        ret(i) = prctile(row, percentage);
    }
    return ret;
}

/**
 *   Return the given percentile on each column
 */
Matrix prctileCols(const Matrix &m, double percentage)
{
    Matrix ret(1, m.nCols(), 0);
    for (int i = 0; i < m.nCols(); i++)
    {
        vector<double> col = m.col(i).colVector();
        ret(i) = prctile(col, percentage);
    }
    return ret;
}

/**
 *  Convenience method for generating plots
 */
void plot(const string &file,
          const Matrix &x,
          const Matrix &y)
{
    LineChart<double> lc;
    lc.setSeries(x.asVector(), y.asVector());
    lc.writeAsHTML(file);
}

/**
 *  Convenience method for generating plots
 */
void hist(const string &file,
          const Matrix &data,
          int numBuckets)
{
    Histogram h;
    h.setData(data.asVector());
    h.writeAsHTML(file);
}

const double ROOT_2_PI = sqrt(2.0 * PI);

static inline double hornerFunction(double x, double a0, double a1)
{
    return a0 + x * a1;
}

static inline double hornerFunction(double x, double a0, double a1, double a2)
{
    return a0 + x * hornerFunction(x, a1, a2);
}

static inline double hornerFunction(double x, double a0, double a1, double a2, double a3)
{
    return a0 + x * hornerFunction(x, a1, a2, a3);
}

static inline double hornerFunction(double x, double a0, double a1, double a2, double a3, double a4)
{
    return a0 + x * hornerFunction(x, a1, a2, a3, a4);
}

static inline double hornerFunction(double x, double a0, double a1, double a2, double a3, double a4,
                                    double a5)
{
    return a0 + x * hornerFunction(x, a1, a2, a3, a4, a5);
}

static inline double hornerFunction(double x, double a0, double a1, double a2, double a3, double a4,
                                    double a5, double a6)
{
    return a0 + x * hornerFunction(x, a1, a2, a3, a4, a5, a6);
}

static inline double hornerFunction(double x, double a0, double a1, double a2, double a3, double a4,
                                    double a5, double a6, double a7)
{
    return a0 + x * hornerFunction(x, a1, a2, a3, a4, a5, a6, a7);
}

static inline double hornerFunction(double x, double a0, double a1, double a2, double a3, double a4,
                                    double a5, double a6, double a7, double a8)
{
    return a0 + x * hornerFunction(x, a1, a2, a3, a4, a5, a6, a7, a8);
}

/**
 *  Arguably this is a little easier to read than the original normcdf
 *  function as it makes the use of horner's method obvious.
 */
double normcdf(double x)
{
    if (x <= 0)
    {
        return 1 - normcdf(-x);
    }
    double k = 1 / (1 + 0.2316419 * x);
    double poly = hornerFunction(k,
                                 0.0, 0.319381530, -0.356563782,
                                 1.781477937, -1.821255978, 1.330274429);
    double approx = 1.0 - 1.0 / ROOT_2_PI * exp(-0.5 * x * x) * poly;
    return approx;
}

/*  Constants required for Moro's algorithm */
static const double a0 = 2.50662823884;
static const double a1 = -18.61500062529;
static const double a2 = 41.39119773534;
static const double a3 = -25.44106049637;
static const double b1 = -8.47351093090;
static const double b2 = 23.08336743743;
static const double b3 = -21.06224101826;
static const double b4 = 3.13082909833;
static const double c0 = 0.3374754822726147;
static const double c1 = 0.9761690190917186;
static const double c2 = 0.1607979714918209;
static const double c3 = 0.0276438810333863;
static const double c4 = 0.0038405729373609;
static const double c5 = 0.0003951896511919;
static const double c6 = 0.0000321767881768;
static const double c7 = 0.0000002888167364;
static const double c8 = 0.0000003960315187;

double norminv(double x)
{
    // We use Moro's algorithm
    double y = x - 0.5;
    if (y < 0.42 && y > -0.42)
    {
        double r = y * y;
        return y * hornerFunction(r, a0, a1, a2, a3) / hornerFunction(r, 1.0, b1, b2, b3, b4);
    }
    else
    {
        double r;
        if (y < 0.0)
        {
            r = x;
        }
        else
        {
            r = 1.0 - x;
        }
        double s = log(-log(r));
        double t = hornerFunction(s, c0, c1, c2, c3, c4, c5, c6, c7, c8);
        if (x > 0.5)
        {
            return t;
        }
        else
        {
            return -t;
        }
    }
}

/**
 *   Evaluate an integral using the rectangle rule
 */
double integral(function<double(double)> f,
                double a,
                double b,
                int nPoints)
{
    double h = (b - a) / nPoints;
    double x = a + 0.5 * h;
    double total = 0.0;
    for (int i = 0; i < nPoints; i++)
    {
        double y = f(x);
        total += y;
        x += h;
    }
    return h * total;
}

double integral2d(function<double(double, double)> f,
                  double a1,
                  double a2,
                  double b1,
                  double b2,
                  int nPoints)
{
    default_random_engine engine1;
    uniform_real_distribution<> unifDistribution1(a1, a2);
    uniform_real_distribution<> unifDistribution2(b1, b2);

    auto gen1 = [&]()
    { return unifDistribution1(engine1); };
    auto gen2 = [&]()
    { return unifDistribution2(engine1); };

    double totalSum = 0;
    for (auto i = 0; i < nPoints; i++)
    {
        double first = gen1();
        double second = gen2();
        totalSum += f(first, second);
    }

    return ((a2 - a1) * (b2 - b1) * totalSum) / static_cast<double>(nPoints);
}

double integral2d(int nThreads,
                  function<double(double, double)> f,
                  double a1,
                  double a2,
                  double b1,
                  double b2,
                  int nPoints)
{
    shared_ptr<Executor> executor = Executor::newInstance(nThreads);
    vector<double> taskSum(nThreads);
    int nPointsTask = (nPoints + nThreads - 1) / nThreads;
    for (auto idx = 0; idx < nThreads; idx++)
    {
        auto taskLambda = [&, idx]()
        {
            int nPointsCurrTask = min(nPoints, (idx + 1) * nPointsTask) - idx * nPointsTask;
            if (nPointsCurrTask == 0)
            {
                return;
            }
            default_random_engine engine;
            engine.discard(2 * 2 * idx * nPointsTask);
            uniform_real_distribution<> unifDistribution1(a1, a2);
            uniform_real_distribution<> unifDistribution2(b1, b2);
            for (int i = 0; i < nPointsCurrTask; i++)
            {
                double first = unifDistribution1(engine);
                double second = unifDistribution2(engine);
                taskSum[idx] += f(first, second);
            }
        };
        executor->addTask(taskLambda);
    }
    executor->join();
    double totalSum = accumulate(taskSum.begin(), taskSum.end(), 0.0);
    return ((a2 - a1) * (b2 - b1) * totalSum) / static_cast<double>(nPoints);
}

/**
 *   Perform a substitution then integate the given
 *   real function from x to infinity by the rectangle rule
 */
double integralToInfinity(function<double(double)> f,
                          double x,
                          int nPoints)
{
    auto i = [&](double s)
    {
        return 1 / (s * s) * f(1 / s + x - 1);
    };
    return integral(i, 0, 1, nPoints);
}

double integralFromInfinity(function<double(double)> f,
                            double x,
                            int nPoints)
{
    auto i = [&](double t)
    {
        return f(-t);
    };
    return integralToInfinity(i, -x, nPoints);
}

double integralOverR(function<double(double)> f,
                     int nPoints)
{
    return integralToInfinity(f, 0, nPoints) + integralFromInfinity(f, 0, nPoints);
}

/**
 *   Bisection method to find x given f(x)
 */
double bisectionMethod(function<double(double)> f,
                       double y,
                       double a,
                       double b,
                       double tolerance)
{
    if (abs(y - f(a)) <= tolerance)
    {
        return a;
    }
    if (abs(y - f(b)) <= tolerance)
    {
        return b;
    }
    ASSERT((f(a) - y) * (f(b) - y) < 0);
    int maxIterations = 50;

    double low = a;
    double high = b;
    while (maxIterations)
    {
        double mid = low + (high - low) / 2;
        if (abs(y - f(mid)) <= tolerance)
        {
            return mid;
        }
        if ((f(low) - y) * (f(mid) - y) < 0)
        {
            high = mid;
        }
        else
        {
            low = mid;
        }
        maxIterations--;
    }
    throw invalid_argument("Provide narrower range OR Increase tolerance");
}

double impliedVolatility(double S, double r, double K, double T, double callOptionPrice, double tolerance)
{
    auto func = [=](double sigma)
    {double numerator = log( S/K ) + ( r + sigma*sigma*0.5)*T;
    double denominator = sigma * sqrt(T );
    double d1 = numerator/denominator;
    double d2 = d1 - denominator;
    return S*normcdf(d1) - exp(-r*T)*K*normcdf(d2); };

    double implVolatility = bisectionMethod(func, callOptionPrice, 0.0, 1.0, tolerance);
    return implVolatility;
}

/**
 *   Creates a matrix of zeros
 */
Matrix zeros(int rows, int cols)
{
    return Matrix(rows, cols);
}

/**
 *   Creates a matrix of ones
 */
Matrix ones(int rows, int cols)
{
    Matrix m = Matrix(rows, cols);
    for (double *p = m.begin(); p != m.end(); p++)
    {
        *p = 1;
    }
    return m;
}

Matrix transpose(const Matrix &in)
{
    Matrix ret(in.nCols(), in.nRows(), false);
    for (int i = 0; i < in.nRows(); i++)
    {
        for (int j = 0; j < in.nCols(); j++)
        {
            ret(j, i) = in(i, j);
        }
    }
    return ret;
}

/*  Compute the cholesky decomposition */
Matrix chol(const Matrix &A)
{
    int n = A.nRows();
    ASSERT(n == A.nCols());
    Matrix L(n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            double s = A(i, j);
            for (int k = 0; k < j; k++)
            {
                s -= L(i, k) * L(j, k);
            }
            L(i, j) = s / L(j, j);
        }
        double s = A(i, i);
        for (int k = 0; k < i; k++)
        {
            s -= L(i, k) * L(i, k);
        }
        ASSERT(s >= 0); /* A must be positive definite */
        L(i, i) = sqrt(s);
    }
    return L;
}

///////////////////////////////////////////////
//
//   TESTS
//
///////////////////////////////////////////////

static Matrix createTestVector()
{
    Matrix v("1;5;3;9;7");
    return v;
}

static void testLinspace()
{
    Matrix result = linspace(1.0, 10.0, 4);
    ASSERT_APPROX_EQUAL(result(0), 1.0, 0.001);
    ASSERT_APPROX_EQUAL(result(1), 4.0, 0.001);
    ASSERT_APPROX_EQUAL(result(2), 7.0, 0.001);
    ASSERT_APPROX_EQUAL(result(3, 0), 10.0, 0.001);
}

static void testSumRows()
{

    Matrix m("1,2,3;4,5,6");
    Matrix expected = Matrix("6;15");
    expected.assertEquals(sumRows(m), 0.001);
    INFO(sumRows(m));

    m = Matrix(10, 10);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            m(i, j) = j;
        }
    }
    Matrix s = sumRows(m);
    Matrix mean = meanRows(m);
    for (int j = 0; j < 10; j++)
    {
        double actual = s(j, 0);
        ASSERT_APPROX_EQUAL(actual, 45, 0.001);
        actual = mean(j, 0);
        ASSERT_APPROX_EQUAL(actual, 4.5, 0.001);
    }
}

static void testSumCols()
{

    Matrix m = Matrix(10, 10);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            m(i, j) = i;
        }
    }
    Matrix s = sumCols(m);
    Matrix mean = meanCols(m);
    for (int i = 0; i < 10; i++)
    {
        double actual = s(0, i);
        ASSERT_APPROX_EQUAL(actual, 45, 0.001);
        actual = mean(0, i);
        ASSERT_APPROX_EQUAL(actual, 4.5, 0.001);
    }
}

static void testStandardDeviation()
{
    ASSERT_APPROX_EQUAL(stdCols(createTestVector()).asScalar(), 3.1623, 0.001);
    ASSERT_APPROX_EQUAL(stdCols(createTestVector(), true).asScalar(), 2.8284, 0.001);
}

static void testRanduniform()
{
    rng("default");
    Matrix m = randuniform(1000, 1);
    ASSERT(m.nRows() == 1000);
    ASSERT_APPROX_EQUAL(meanCols(m).asScalar(), 0.5, 0.1);
    ASSERT(maxOverCols(m).asScalar() < 1.0);
    ASSERT(minOverCols(m).asScalar() > 0.0);
}

static void testRandn()
{
    rng("default");
    Matrix m = randn(10000, 1);
    ASSERT(m.nRows() == 10000);
    ASSERT_APPROX_EQUAL(meanCols(m).asScalar(), 0.0, 0.1);
    ASSERT_APPROX_EQUAL(stdCols(m).asScalar(), 1.0, 0.1);
}

static void testNormCdf()
{
    ASSERT_APPROX_EQUAL(normcdf(1.96), 0.975, 0.001);
}

static void testNormInv()
{
    ASSERT_APPROX_EQUAL(norminv(0.975), 1.96, 0.01);
}

static void testPrctile()
{
    const vector<double> v = createTestVector().colVector();
    ASSERT_APPROX_EQUAL(prctile(v, 100.0), 9.0, 0.001);
    ASSERT_APPROX_EQUAL(prctile(v, 0.0), 1.0, 0.001);
    ASSERT_APPROX_EQUAL(prctile(v, 50.0), 5.0, 0.001);
    ASSERT_APPROX_EQUAL(prctile(v, 17.0), 1.7, 0.001);
    ASSERT_APPROX_EQUAL(prctile(v, 62.0), 6.2, 0.001);

    Matrix m("1,2,3;4,5,6");
    Matrix("2;5").assertEquals(prctileRows(m, 50.0), 0.001);
    m = Matrix("1,2,3;4,5,6");
    Matrix("2.5,3.5,4.5").assertEquals(prctileCols(m, 50.0), 0.001);
}

/*  To test the integral function, we need a function
    to integrate */
class SinFunction : public RealFunction
{
    double evaluate(double x);
};

double SinFunction::evaluate(double x)
{
    return sin(x);
}

static void testIntegral()
{
    SinFunction integrand;
    double actual = integral(integrand, 1, 3, 1000);
    double expected = -cos(3.0) + cos(1.0);
    ASSERT_APPROX_EQUAL(actual, expected, 0.000001);
}

double randuniform(default_random_engine &random)
{
    return (random() + 0.5) / (random.max() + 1.0);
}

static void testRandomEngine()
{
    mt19937 e1;
    uniform_real_distribution<> dist(0.0, 10.0);
    auto iterations = 4;
    vector<double> sumVec(iterations);
    double totalSum = 0;
    for (auto i = 0; i < iterations; i++)
    {
        double val = dist(e1);
        sumVec[i] = val;
    }
    totalSum = accumulate(sumVec.begin(), sumVec.end(), 0.0);

    auto nThreads = 4;
    vector<double> sumThread(nThreads);
    vector<thread> threadVec(nThreads);
    auto iterationsThread = (iterations + nThreads - 1) / nThreads;
    for (auto idx = 0; idx < nThreads; idx++)
    {
        threadVec[idx] = thread([&, idx]()
                                {
            mt19937 e;
            uniform_real_distribution<> dist(0.0, 10.0);
            e.discard(2 * idx * iterationsThread);
            for(auto i = 0; i < iterationsThread; i++)
            {
                double val = dist(e);
                sumThread[idx] += val;
            } });
    }

    for (auto idx = 0; idx < nThreads; idx++)
    {
        threadVec[idx].join();
    }

    double threadTotalSum = accumulate(sumThread.begin(), sumThread.end(), 0.0);
    ASSERT_APPROX_EQUAL(totalSum, threadTotalSum, 0.1);
}

static void testIntegral2d()
{
    auto func = [](double x, double y)
    { return x * x + 4 * y; };
    auto singleThreadedres = integral2d(func, 11, 14, 7, 10, 10000);

    auto multiThreadedres = integral2d(10, func, 11, 14, 7, 10, 10000);
    ASSERT_APPROX_EQUAL(multiThreadedres, singleThreadedres, 1);
}

/**
 *  When you create a small class like this, using
 *  nested classes is easier.
 */
static void testIntegralVersion2()
{

    class Sin : public RealFunction
    {
        double evaluate(double x)
        {
            return sin(x);
        }
    };

    Sin integrand;
    double actual = integral(integrand, 1, 3, 1000);
    double expected = -cos(3.0) + cos(1.0);
    ASSERT_APPROX_EQUAL(actual, expected, 0.000001);
}

static void testIntegral3()
{
    auto integrand = [](double x)
    {
        return sqrt(1 + pow(sin(x), 2.0));
    };
    double res = integral(integrand, 0, PI, 1000);
    ASSERT_APPROX_EQUAL(res, 3.820197789, 0.001);
}

static void testInfiniteIntegrals()
{
    auto normPDF = [](double x)
    {
        return 1 / ROOT_2_PI * exp(-0.5 * x * x);
    };
    ASSERT_APPROX_EQUAL(integralOverR(normPDF, 1000), 1.0, 0.01);
}

static void testBisectMethod()
{
    function<double(double)> func = [](double x)
    { return x * x; };
    double res = bisectionMethod(func, 9, 1, 25, 0.1);
    ASSERT_APPROX_EQUAL(res, 3.0, 0.1);
    res = bisectionMethod(func, 5.76, 1, 25, 0.1);
    ASSERT_APPROX_EQUAL(res, 2.4, 0.1);
    function<double(double)> trigoFunc = [](double x)
    { return sin(x) + cos(x); };
    res = bisectionMethod(trigoFunc, -0.569681, -2.5, 1, 0.01);
    ASSERT_APPROX_EQUAL(res, -1.2, 0.1);
}

static void testImpliedVolatility()
{
    double volatility = impliedVolatility(100.0, 0.05, 105.0, 1.0, 4.046, 0.01);
    ASSERT_APPROX_EQUAL(volatility, 0.1, 0.01);
    volatility = impliedVolatility(100.0, 0.05, 105.0, 1.0, 8.02136, 0.01);
    ASSERT_APPROX_EQUAL(volatility, 0.2, 0.01);
    // volatility = impliedVolatility(4768.0, 0.0380, 3600.0, 1.0, 1343.4, 10.0);
    // ASSERT_APPROX_EQUAL(volatility, 0.3397, 0.01);
}

static void testMinOverRows()
{
    Matrix m("1,2,3;4,5,6");
    Matrix expected("1;4");
    expected.assertEquals(minOverRows(m), 0.001);
}

static void testMaxOverRows()
{
    Matrix m("1,2,3;4,5,6");
    Matrix expected("3;6");
    INFO("Expected " << expected);
    INFO("Actual " << maxOverRows(m));
    expected.assertEquals(maxOverRows(m), 0.001);
}

static void testMinOverCols()
{
    Matrix m("1,2,3;4,5,6");
    Matrix expected("1,2,3");
    expected.assertEquals(minOverCols(m), 0.001);
}

static void testMaxOverCols()
{
    Matrix m("1,2,3;4,5,6");
    Matrix expected("4,5,6");
    expected.assertEquals(maxOverCols(m), 0.001);
}

static void testSortRows()
{
    Matrix m("3,2,1");
    Matrix expected("1,2,3");
    expected.assertEquals(sortRows(m), 0.001);
}

static void testSortCols()
{
    Matrix m("3;2;1");
    Matrix expected("1;2;3");
    expected.assertEquals(sortCols(m), 0.001);
}

static void testTranspose()
{
    Matrix m("3;2;1");
    Matrix expected("3,2,1");
    expected.assertEquals(transpose(m), 0.001);
}

static void testChol()
{
    Matrix m("3,1,2;1,4,-1;2,-1,5");
    Matrix c = chol(m);
    Matrix product = c * transpose(c);
    m.assertEquals(product, 0.001);
}

void testMatlib()
{
    TEST(testLinspace);
    TEST(testSumRows);
    TEST(testSumCols);
    TEST(testStandardDeviation);
    TEST(testRanduniform);
    TEST(testRandn);
    TEST(testNormInv);
    TEST(testNormCdf);
    TEST(testPrctile);
    TEST(testIntegral);
    TEST(testRandomEngine);
    TEST(testIntegral2d);
    TEST(testIntegralVersion2);
    TEST(testInfiniteIntegrals);
    TEST(testBisectMethod);
    TEST(testImpliedVolatility);
    TEST(testMaxOverRows);
    TEST(testMinOverRows);
    TEST(testMaxOverCols);
    TEST(testMinOverCols);
    TEST(testSortRows);
    TEST(testSortCols);
    TEST(testTranspose);
    TEST(testChol);
    TEST(testIntegral3);
}
