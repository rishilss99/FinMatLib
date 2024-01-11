#pragma once

#include "stdafx.h"
#include "Matrix.h"


/*  Create a linearly spaced vector */
Matrix linspace( double from, double to, int numPoints, bool rowVector=0 );
/*  Compute the sum of a matrix's rows */
Matrix sumRows( const Matrix& m );
/*  Compute the sum of a matrix's cols */
Matrix sumCols( const Matrix& m );
/*  Compute the mean of a matrix's rows */
Matrix meanRows( const Matrix& m );
/*  Compute the mean of a matrix's cols */
Matrix meanCols( const Matrix& m );
/*  Compute the standard deviation of a matrix's rows */
Matrix stdRows( const Matrix& m, bool population=0 );
/*  Compute the standard deviation of a matrix's rows */
Matrix stdCols( const Matrix& m, bool population=0 );
/*  Compute the minimum entry of each row */
Matrix minOverRows( const Matrix& m );
/*  Compute the minimum entry of each col */
Matrix minOverCols( const Matrix& m );
/*  Compute the maximum entry of each row */
Matrix maxOverRows( const Matrix& m );
/*  Compute the maximum entry of each col */
Matrix maxOverCols( const Matrix& m );
/*  Find the given percentile over the rows of a vector */
Matrix prctileRows( const std::vector<double>& v, double percentage );
/*  Find the given percentile over the cols of a vector */
Matrix prctileCols( const std::vector<double>& v, double percentage );
/*  Sort the rows of a matrix */
Matrix sortRows( const Matrix&  m );
/*  Sort the cols of a matrix */
Matrix sortCols( const Matrix&  m );


/*  Create uniformly distributed random numbers */
Matrix randuniform( int rows, int cols );
/*  Create normally distributed random numbers */
Matrix randn( int rows, int cols );
/*  Create uniformly distributed random numbers */
Matrix randuniform(std::mt19937& random,
				  int rows, int cols);
/*  Create normally distributed random numbers */
Matrix randn(std::mt19937& random,
			 int rows, int cols);
/*  Seeds the default random number generator */
void rng( const std::string& setting );

/**
 *  Exponentiate a matrix
 */
inline Matrix exp(const Matrix& m ) {
	Matrix ret = m;
	ret.exp();
	return ret;
}

/**
 *  Pointwise product
 */
inline Matrix dotTimes(Matrix& a, const Matrix& b) {
	Matrix ret = a;
	ret.times(b);
	return ret;
}


/*  Matrix transpose */
Matrix transpose(const Matrix& m);
/*  Cholesky decomposition */
Matrix chol(const Matrix& m);


/**
 *  Computes the cumulative
 *  distribution function of the
 *  normal distribution
 */
double normcdf( double x );

/* Computes the inverse of normcdf */
double norminv( double x ); 


/*  Create a line chart given vectors x and y */
void plot( const std::string& fileName,
           const Matrix& x,
           const Matrix& y);

/*  Plot a histogram */
void hist( const std::string& fileName,
           const Matrix& values,
           int numBuckets=10);

/*  Integrate using the rectangle rule */
double integral( std::function<double(double)> f,
                 double a,
                 double b,
                 int nSteps );

/*  Integrate f from x to infinity using a substitution
followed by the rectangle rule */
double integralToInfinity(std::function<double(double)> f,
	double x,
	int nSteps);

/*  Integrate f over the whole of R
by two applications of integral to Infinity */
double integralOverR(std::function<double(double)> f,
	int nSteps);


/**
 *   Returns the implied volatility given the parameters for a BSM call option
*/
double impliedVolatility(double S, double r, double K, double T, double callOptionPrice, double tolerance);

/**
 *   Creates a matrix of zeros
 */
Matrix zeros( int rows, int cols );

/**
 *   Creates a matrix of ones
 */
Matrix ones( int rows, int cols );





/**
 *  Test function
 */
void testMatlib();

