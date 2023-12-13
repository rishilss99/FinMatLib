#pragma once

#include "stdafx.h"

class Matrix {
public:

	/*  Container conventions */
    typedef double value_type;
	typedef double* iterator;
	typedef const double* const_iterator;

    /**
     *  Constructs a matrix. By default all values
     *  are set to zero
     */
    Matrix( int nrows, int ncols, bool zeros=1 );
    /**
     *  Default constructor
     */
    Matrix();
    /*  Construct a matrix using a string of data */
    explicit Matrix( std::string data );
    /*  Create a 1 by 1 matrix */
    explicit Matrix( double value );
    /*  Create a vector */
    explicit Matrix( std::vector<double> data, bool rowVector=0 );

    /**
     *  Destructor, cleans up the data created
     */
    ~Matrix() {
        delete[] data;
    }

    /**
     * Retrieve the value at the given index
     */
    double get( int i, int j ) const {
        return data[ offset(i, j ) ];
    }

    /**
     *  Set the value at the given index
     */
    void set( int i, int j, double value ) {
        data[ offset(i, j ) ] = value;
    }

    /**
     *   The number of rows in the matrix
     */
    int nRows() const {
        return nrows;
    }

    /**
     *  The number of columns in the matrix
     */
    int nCols() const {
        return ncols;
    }

    /**
     *   Allows one to access a cell using parentheses
     *   Apparently using round brackets rather than square
     *   ones is preferable in terms of speed!
     */
    double& operator()(int i, int j ) {
        return data[ offset(i,j) ];
    }

    /**
     *   If you want a reference to something inside a const
     *   object, the returned reference must be const
     */
    const double& operator()(int i, int j ) const {
        return data[ offset(i,j) ];
    }

    /**
     *   Allows one to access a cell of a vector using parentheses
     */
    double& operator()(int i ) {
        ASSERT( i<nrows*ncols );
        return  data[ i ];
    }

    /**
     *   Allows one to access a cell of a vector using parentheses
     */
    const double& operator()(int i) const {
        ASSERT( i<nrows*ncols );
        return data[ i ];
    }


    /**
     *   The assignment operator must be implemented by the rule
     *   of three
     */
    Matrix& operator=( const Matrix& other ) {
        delete[] data;
        assign( other );
        return *this;
    }

    /**
     *   This must be implemented by the rule of three
     */
    Matrix( const Matrix& other ) {
        assign( other );
    }

    /*  Access a pointer to the first element */
    const double* begin() const {
        return data;
    }
    /*  Access a pointer to the element after last */
    const double* end() const {
        return endPointer;
    }
    /*  Access a pointer to the first element */
    double* begin() {
        return data;
    }
    /*  Access a pointer to the element after last */
    double* end() {
        return endPointer;
    }

    /*  
     *   Assert two matrices are identical
     */    
    void assertEquals( const Matrix& other, double tolerance );

    /*  Exponentiate every element */
    void exp();
    /*  Square root every element */
    void sqrt();
    /*  Take the log of every element */
    void log();
    /*  Take the positive part of every element */
    void positivePart();
    /*  Take the negative part of every element */
    void negativePart();


    /*  Entrywise raising to a power */
    void pow( double power );
    /*  Entrywise raising to a power */
    void pow( const Matrix& power );

    /*  Entrywise multiplication */
    inline void times( double factor ) {
        (*this)*=factor;
    }
    /*  Entrywise multiplication */
    void times( const Matrix& other );
    /*  Tests the value of each cell and replaces the value with valueIfTrue or valueIfFalse
        according to whether the current value is 1 or 0 */
    void test( const Matrix& valueIfTrue, const Matrix& valueIfFalse );


    /*  Scalar multiplication */
    Matrix& operator*=( double factor );
    /*  Scalar addition */
    Matrix& operator+=( double scalar );
    /*  Addition */
    Matrix& operator+=( const Matrix& other );
    /*  Scalar subtraction */
    Matrix& operator-=( double scalar );
    /*  Subtraction */
    Matrix& operator-=( const Matrix& other );

    /*  Assign a column to match a column in another matrix */
    void setCol( int col, const Matrix& other, int otherCol);
    /*  Assign a column to match a row in another matrix */
    void setRow( int row, const Matrix& other, int otherRow);

    /*  Convert a row vector to a std::vector<double> */
    std::vector<double> rowVector() const;
    /*  Convert a column vector to a std::vector<double> */
    std::vector<double> colVector() const;
    /*  Convert a row or column vector into a std::vector<double> */
    std::vector<double> asVector() const;

    /*  Converts a 1x1 matrix to a scalar */
    double asScalar() const {
        ASSERT( nrows==1 && ncols==1);
        return *data;
    }

    /*  Returns a matrix representing the given row */
    Matrix row( int row ) const ;
    /*  Returns a matrix representing the given column */
    Matrix col( int col ) const ;

    /*  
     *  Returns the offset to the given cell in a matrix
     */
    int offset( int i, int j ) const {
        // Note that this assert is not tested when running in the release mode
        ASSERT( i >=0 && i<nrows && j>=0 && j<ncols );
        return j*nrows + i;
    }

private:
    
    /*  The number of rows in the matrx */
    int nrows;
    /*  The number of columns */
    int ncols;
    /*  The data in the matrix */
    double* data;
    /*  Pointer to one after the end of the data */
    double* endPointer;


    /**
     *  Assign values to this matrix so that it contains
     * the same data as another matrix
     */
    void assign( const Matrix& other );
};

/*  Define shared ptr to a matrix type */
typedef std::shared_ptr<Matrix> SPMatrix;
typedef std::shared_ptr<const Matrix> SPCMatrix;

/*  Write a matrix to a stream */
std::ostream& operator<<(std::ostream& out, const Matrix& m );

/*  Multiply a matrix by a scalar */
Matrix operator*(const Matrix& m, double scalar );

/*  Matrix multiplication */
Matrix operator*(const Matrix& a, const Matrix& b);

/*  Multiply a matrix by a scalar */
inline Matrix operator*(double scalar, const Matrix& m ) {
    return m*scalar;
}

/*  Add a scalar to every element of a matrix */
Matrix operator+(const Matrix& m, double scalar );

/*  Add a scalar to every element of a matrix */
inline Matrix operator+(double scalar, const Matrix& m ) {
    return m+scalar;
}

/*  Add two matrices */
Matrix operator+(const Matrix& x, const Matrix& y );
/*  Subtraction */
Matrix operator-(double scalar, const Matrix& m );
/*  Subtract a scalar from a matrix */
Matrix operator-(const Matrix& m, double scalar );
/*  Subtract two matrices */
Matrix operator-(const Matrix& x, const Matrix& y );

/*  Comparison operator */
Matrix operator>(const Matrix& x, double s );
/*  Comparison operator */
Matrix operator>=(const Matrix& x, double s );
/*  Comparison operator */
Matrix operator<(const Matrix& x, double s );
/*  Comparison operator */
Matrix operator<=(const Matrix& x, double s );
/*  Comparison operator */
Matrix operator==(const Matrix& x, double s );
/*  Comparison operator */
Matrix operator!=(const Matrix& x, double s);

/*  Comparison operator */
inline Matrix operator>(double s, const Matrix& x  ) {
    return x<s;
}
/*  Comparison operator */
inline Matrix operator>=(double s, const Matrix& x ) {
    return x<=s;
}
/*  Comparison operator */
inline Matrix operator<(double s, const Matrix& x ) {
    return x>s;
}
/*  Comparison operator */
inline Matrix operator<=(double s, const Matrix& x ) {
    return x>=s;
};
/*  Comparison operator */
inline Matrix operator==(double s, const Matrix& x ) {
    return x==s;
}
/*  Comparison operator */
inline Matrix operator!=(double s, const Matrix& x ) {
    return x!=s;
}
/*  Comparison operator */
Matrix operator>(const Matrix& x, const Matrix& s );
/*  Comparison operator */
Matrix operator>=(const Matrix& x,  const Matrix&  s );
/*  Comparison operator */
Matrix operator<(const Matrix& x,  const Matrix&  s );
/*  Comparison operator */
Matrix operator<=(const Matrix& x,  const Matrix&  s );
/*  Comparison operator */
Matrix operator==(const Matrix& x,  const Matrix&  s );
/*  Comparison operator */
Matrix operator!=(const Matrix& x,  const Matrix&  s);






///////////////////////////////
//
//   TESTS
//
///////////////////////////////


void testMatrix();