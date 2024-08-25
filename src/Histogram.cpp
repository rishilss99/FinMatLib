#include "Histogram.h"
#include "testing.h"
#include "matlib.h"
#include "LineChart.h"

extern template class LineChart<double>;

using namespace std;




/**
*    This function computes the data needed to plot a histogram
*    we have two vectors - one containing numBuckets+1 values which
*    indicate the x coordinates of each "fence post" in our histogram
*    and another containing numBuckets values which contains the heights
*    of each fence post.
*
*   All vectors are col vectors
*/
static void histogramData(const Matrix& values,
	int numBuckets,
	Matrix& fencePosts,
	Matrix& heights) {

	fencePosts = Matrix(numBuckets + 1, 1);
	heights = Matrix(numBuckets, 1);

	ASSERT(values.nRows()>0);
	Matrix sortedValues = sortCols(values);

	double start = minOverCols(values).asScalar();
	double end = maxOverCols(values).asScalar();

	double width = (end - start) / numBuckets;
	for (int i = 0; i<numBuckets + 1; i++) {
		fencePosts(i, 0) = start + i*width;
	}
	fencePosts(numBuckets) = end;
	int currentFence = 0;
	int numPoints = sortedValues.nRows();
	double fenceEnd = fencePosts(currentFence + 1, 0);
	for (int i = 0; i<numPoints; i++) {
		double value = sortedValues(i, 0);
		while (value>fenceEnd) {
			currentFence++;
			fenceEnd = fencePosts(currentFence + 1, 0);
		}
		heights(currentFence, 0)++;
	}

}

/**
*  Having computed the fence posts and fence heights for our histogram
*  we can convert this to a series of x and y points to plot. Google charts
*  works fine with repeated x points, so this will give a nice looking result.
*/
static void fencePostsToPlotPoints(const Matrix& fencePosts,
	const Matrix& heights,
	Matrix& xPoints,
	Matrix& yPoints) {
	int n = heights.nRows();
	xPoints = Matrix(4 * n, 1);
	yPoints = Matrix(4 * n, 1);
	int count = 0;
	for (int i = 0; i<n; i++) {
		double x1 = fencePosts(i, 0);
		double x2 = fencePosts(i + 1, 0);
		double y = heights(i, 0);
		xPoints(count, 0) = x1;
		yPoints(count, 0) = 0.0;
		count++;
		xPoints(count, 0) = x1;
		yPoints(count, 0) = y;
		count++;
		xPoints(count, 0) = x2;
		yPoints(count, 0) = y;
		count++;
		xPoints(count, 0) = x2;
		yPoints(count, 0) = 0;
		count++;
	}
	ASSERT(count == 4 * n);
}


//////////////////////////////////
// Histogram class
/////////////////////////////////

Histogram::Histogram() :
numBuckets(10),
title("A Histogram") {
}

void Histogram::setTitle(const string& t) {
	title = t;
}

void Histogram::setData(const vector<double>& v) {
	data = Matrix(v);
}

void Histogram::setNumBuckets(int n) {
	numBuckets = n;
}


void Histogram::writeAsHTML(ostream& out) const {
	Matrix fencePosts, heights, x, y;
	histogramData(data, numBuckets, fencePosts, heights);
	fencePostsToPlotPoints(fencePosts, heights, x, y);
	LineChart<double> lc;
	lc.setTitle(title);
	lc.setSeries(x.colVector(), y.colVector());
	lc.writeAsHTML(out);
}

void Histogram::writeAsHTML(const string& file) const {
    string file_path = "output/" + file;
    ofstream out;
    out.open(file_path.c_str());
	out.close();
}




//////////////////////////////////////////////////////
//
//   TESTS
//
//////////////////////////////////////////////////////



void testHistogramData() {
	// a good test that is completely automated
	Matrix data("1;2;3;4;5");

	Matrix fencePosts;
	Matrix heights;
	histogramData(data,
		3,
		fencePosts,
		heights);

	ASSERT(fencePosts.nRows() == 4);
	ASSERT(heights.nRows() == 3);
	ASSERT_APPROX_EQUAL(fencePosts(0), 1.0, 0.001);
	ASSERT_APPROX_EQUAL(fencePosts(3), 5.0, 0.001);
	ASSERT_APPROX_EQUAL(sumCols(Matrix(heights)).asScalar(), 5.0, 0.001);
}

void testFencePostsToPlotPoints() {
	// a good test that is completely automated
	Matrix fencePosts("1;2;3");
	Matrix heights("1.0;-1.0");

	Matrix x, y;
	fencePostsToPlotPoints(fencePosts, heights, x, y);

	ASSERT_APPROX_EQUAL(x(0), 1.0, 0.001);
	ASSERT_APPROX_EQUAL(x(1), 1.0, 0.001);
	ASSERT_APPROX_EQUAL(x(2), 2.0, 0.001);
	ASSERT_APPROX_EQUAL(x(3), 2.0, 0.001);
	ASSERT_APPROX_EQUAL(x(4), 2.0, 0.001);

	ASSERT_APPROX_EQUAL(y(0), 0.0, 0.001);
	ASSERT_APPROX_EQUAL(y(1), 1.0, 0.001);
	ASSERT_APPROX_EQUAL(y(2), 1.0, 0.001);
	ASSERT_APPROX_EQUAL(y(3), 0.0, 0.001);
	ASSERT_APPROX_EQUAL(y(4), 0.0, 0.001);
	ASSERT_APPROX_EQUAL(y(5), -1.0, 0.001);

}

static void testAll() {
	// a somewhat unsatisfactory test since it writes a file, but pretty much
	// everything it uses has been tested individually.
	Histogram h;
	h.setTitle("Uniform distribution");
	h.setData(randuniform(1000, 1).colVector());
	h.writeAsHTML("uniformDistributionHistogram.html");
}

void testHistogram() {
	TEST(testHistogramData);
	TEST(testFencePostsToPlotPoints);
	TEST(testAll);
}