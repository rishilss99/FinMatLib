#include "Matrix.h"
#include "matlib.h"

using namespace std;

/**
 *  Initializes a matrix using a string in the format
 *  1,2,3;4,5,6 etc.
 */
Matrix::Matrix( string s ) {
    char separator;
    // read once to compute the size
    nrows = 1;
    ncols = 1;
    int n = s.size();
    for (int i=0; i<n; i++) {
        if (s[i]==';') {
            nrows++;
        }
        if (nrows==1 && s[i]==',') {
            ncols++;
        }
    }

    // now check we can read the string
    stringstream ss1;
    ss1.str(s);
    for (int i=0; i<nrows; i++) {
        for (int j=0; j<ncols; j++) {
            double ignored;
            ss1 >> ignored;
            ss1 >> separator;
            if (j==ncols-1 && i<nrows-1) {
                ASSERT( separator==';' );
            } else if (j<ncols-1) {
                ASSERT( separator==',' );
            }
        }
    }

    // allocate memory now we know nothing will go wrong
    stringstream ss;
    ss.str(s);
    int size = nrows*ncols;
    data = new double[size];
    endPointer = data+size;
    for (int i=0; i<nrows; i++) {
        for (int j=0; j<ncols; j++) {
            double* p = begin() + offset( i,j );
            ss >> (*p);
            ss >> separator;
        }
    }
}

Matrix::Matrix( int nrows, int ncols, bool zeros )
    : nrows( nrows ), ncols( ncols ) {
    int size = nrows*ncols;
    data = new double[size];
    endPointer = data+size;
    if (zeros) {
        // memset is an optimised low level function
        // that should be faster than looping
        memset( data, 0, sizeof( double )*size );
    }
};

Matrix::Matrix()
    : nrows( 1 ), ncols( 1 ) {
    int size = nrows*ncols;
    data = new double[size];
    endPointer = data+size;
    *data = 0.0;
};

Matrix::Matrix( double value )
    : nrows( 1 ), ncols( 1 ) {
    int size = nrows*ncols;
    data = new double[size];
    endPointer = data+size;
    *data = value;
};

Matrix::Matrix( std::vector<double> vals, bool rowVector )
    : nrows( vals.size()), ncols(1) {
    if (rowVector) {
        ncols = vals.size();
        nrows = 1;
    }
    int size = nrows*ncols;
    data = new double[size];
    endPointer = data+size;
    for (int i=0; i<size; i++) {
        data[i] = vals[i];
    }
}



/**
 *  Assign all the member variables of this matrix
 *  so that they match another matrix
 */
void Matrix::assign( const Matrix& other ) {
    nrows = other.nrows;
    ncols = other.ncols;
    int size = nrows*ncols;
    data = new double[size];
    endPointer = data+size;
    memcpy( data, other.data, sizeof( double )*size );
}


/*  
 *   Assert two matrices are identical
 */
void Matrix::assertEquals( const Matrix& other, double tolerance ) {
    ASSERT( other.nrows == nrows );
    ASSERT( other.ncols == ncols );
    for (int i=0; i<nrows; i++) {
        for (int j=0; j<ncols; j++) {
            double expected = (*this)(i,j);
            double actual = other(i,j);
            if (fabs( expected-actual )>tolerance) {
                stringstream s;
                s << "ASSERTION FAILED\n";
                s << "Mismatch at index "<<i<<", "<<j<<". ";
				s << "Expected "<<expected<<", actual "<<actual<<"\n";
				s << "this= "<<(*this)<<"\n";
				s << "other="<<other<<"\n";
				INFO(s.str());
                throw std::runtime_error( s.str() );
            }
        }
    }
}

/**
 *   If the matrix is a row vector, convert it into an std::vector
 */
vector<double> Matrix::rowVector() const {
    ASSERT( nrows == 1);
    vector<double> ret(ncols);
    for (int i=0; i<ncols; i++) {
        ret[i]=(*this)(0,i);
    }
    return ret;
}



/**
 *   If the matrix is a column vector, convert it into an std::vector
 */
vector<double> Matrix::colVector() const {
    ASSERT( ncols == 1);
    vector<double> ret(nrows);
    for (int i=0; i<nrows; i++) {
        ret[i]=(*this)(i,0);
    }
    return ret;
}

/*  Convert a row or column vector into a std::vector<double> */
vector<double> Matrix::asVector() const {
    if (nrows==1) {
        return rowVector();
    } else {
        return colVector();
    }
}


/**
 *   Set a column to match a column in another matrix
 */
void Matrix::setCol( int col, const Matrix& other, int otherCol ) {
    ASSERT( other.nrows == nrows );
    double* dest = begin()+offset(0,col);
    const double* src = other.begin() + other.offset(0,otherCol);
    memcpy( dest, src, sizeof(double)*nrows);
}

/**
 *   Set a column to match a column in another matrix
 */
void Matrix::setRow( int row, const Matrix& other, int otherRow ) {
    ASSERT( other.ncols == ncols );
    double* dest = begin()+ offset(row,0);
    const double* src = other.begin() + other.offset(otherRow,0);
    for (int i=0; i<ncols; i++) {
        *dest = *src;
        dest+=nrows;
        src+=other.nrows;
    }
}

/**
 *   Returns the row vector corresponding to the given row
 */
Matrix Matrix::row( int i ) const {
    Matrix r(1,ncols, 0);
    r.setRow(0,*this,i);
    return r;
}

/**
 *   Returns the col vector corresponding to the given column
 */
Matrix Matrix::col( int j ) const {
    Matrix r(nrows,1, 0);
    r.setCol(0,*this,j);
    return r;
}



/*  Exponentiate every element */
void Matrix::exp() {
	for (double* p=begin(); p!=end(); p++) {
		*p = std::exp(*p);
	}
}
/*  Square root every element */
void Matrix::sqrt() {
	for (double* p=begin(); p!=end(); p++) {
		*p = std::sqrt(*p);
	}
}
/*  Take the log of every element */
void Matrix::log() {
	for (double* p=begin(); p!=end(); p++) {
		*p = std::log(*p);
	}
}

/*  Entrywise raising to a power */
void Matrix::pow( double power ) {
	for (double* p=begin(); p!=end(); p++) {
		*p = std::pow(*p, power);
	}
}

/*  Take the positive part of every element in the matrix */
void Matrix::positivePart() {
	for (double* p=begin(); p!=end(); p++) {
        double val = *p;
		*p = (val>0.0) ? val : 0.0;
	}
}
/*  Take the negative part of every element in the matrix */
void Matrix::negativePart() {
	for (double* p=begin(); p!=end(); p++) {
        double val = *p;
		*p = (val<0.0) ? val : 0.0;
	}
}


/*  Entrywise raising to a power */
void Matrix::pow( const Matrix& power ) {
    ASSERT( nRows()==power.nRows() && nCols()==power.nCols());
	double* p1=begin();
	const double* p2=power.begin();
	while (p1!=end()) {
		*p1=std::pow(*p1,*p2);
		p1++;
		p2++;
	}
}

/*  Entrywise multiplication */
void Matrix::times( const Matrix& factor ) {
    ASSERT( nRows()==factor.nRows() && nCols()==factor.nCols());
	double* p1=begin();
	const double* p2=factor.begin();
	while (p1!=end()) {
		*p1=(*p1) * (*p2);
		p1++;
		p2++;
	}
}

/*  Test if the cells of this matrix are 1 or 0 and then replace the values with    
    valueIfTrue and valueIfFalse accordingly */
void Matrix::test( const Matrix& valueIfTrue, const Matrix& valueIfFalse ) {
    double* p = begin();
    const double* trueP = valueIfTrue.begin();
    const double* falseP = valueIfFalse.begin();
	while(p!=end()) {
        double value = *p;
		*p = value * (*trueP) + (!value) * (*falseP);
        trueP++;
        falseP++;
        p++;
	}
}


/*  Scalar multiplication */
Matrix& Matrix::operator*=( double scalar ) {
	for (double* p=begin(); p!=end(); p++) {
		*p = (*p) * scalar;
	}
	return *this;
}
/*  Scalar addition */
Matrix& Matrix::operator+=( double scalar ) {
	for (double* p=begin(); p!=end(); p++) {
		*p = *p + scalar;
	}
	return *this;
}
/*  Addition */
Matrix& Matrix::operator+=( const Matrix& other ) {
    ASSERT( nRows()==other.nRows() && nCols()==other.nCols());
	double* p1=begin();
	const double* p2=other.begin();
	while (p1!=end()) {
		*p1=(*p1) + (*p2);
		p1++;
		p2++;
	}
	return *this;
}
/*  Scalar subtraction */
Matrix& Matrix::operator-=( double scalar ) {
	for (double* p=begin(); p!=end(); p++) {
		*p = *p - scalar;
	}
	return *this;
}

/*  Subtraction */
Matrix& Matrix::operator-=( const Matrix& other ) {
    ASSERT( nRows()==other.nRows() && nCols()==other.nCols());
	double* p1=begin();
	const double* p2=other.begin();
	while (p1!=end()) {
		*p1=(*p1) - (*p2);
		p1++;
		p2++;
	}
	return *this;
}

ostream& operator<<(ostream& out, const Matrix& m ) {
	int nRow = m.nRows();
	int nCol = m.nCols();
	out <<"[";
	for (int i=0; i<nRow; i++) {
		for (int j=0; j<nCol; j++) {
			out << m(i,j);
			if (j!=nCol-1) {
				out << ",";
			}
		}
		if (i!=nRow-1) {
			out << ";";
		}
	}
	out <<"]";
	return out;
}

Matrix operator*(const Matrix& m, double scalar ) {
    Matrix ret(m.nRows(), m.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = m.begin();
    const double* end = m.end();
    while (source!=end) {
        *(dest++) = *(source++) * scalar;
    }
    return ret;
}


Matrix operator+(const Matrix& m, double scalar ) {
    Matrix ret(m.nRows(), m.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = m.begin();
    const double* end = m.end();
    while (source!=end) {
        *(dest++) = *(source++) + scalar;
    }
    return ret;
}


Matrix operator+(const Matrix& x, const Matrix& y ) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0 );
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) + *(s2++);
    }
    return ret;
}

Matrix operator-(double scalar, const Matrix& m ) {
    Matrix ret(m.nRows(), m.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = m.begin();
    const double* end = m.end();
    while (source!=end) {
        *(dest++) = scalar - *(source++);
    }
    return ret;
}

Matrix operator-(const Matrix& m, double scalar ) {
    Matrix ret(m.nRows(), m.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = m.begin();
    const double* end = m.end();
    while (source!=end) {
        *(dest++) = *(source++) - scalar;
    }
    return ret;
}

Matrix operator-(const Matrix& x, const Matrix& y ) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0);
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) - *(s2++);
    }
    return ret;
}


/*  Comparison operator */
Matrix operator>(const Matrix& x, double s ) {
    Matrix ret(x.nRows(), x.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = x.begin();
    const double* end = x.end();
    while (source!=end) {
        *(dest++) = *(source++) > s;
    }
    return ret;
}
/*  Comparison operator */
Matrix operator>=(const Matrix& x, double s ) {
    Matrix ret(x.nRows(), x.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = x.begin();
    const double* end = x.end();
    while (source!=end) {
        *(dest++) = *(source++) >= s;
    }
    return ret;
}
/*  Comparison operator */
Matrix operator<(const Matrix& x, double s ) {
    Matrix ret(x.nRows(), x.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = x.begin();
    const double* end = x.end();
    while (source!=end) {
        *(dest++) = *(source++) < s;
    }
    return ret;
}
/*  Comparison operator */
Matrix operator<=(const Matrix& x, double s ) {
    Matrix ret(x.nRows(), x.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = x.begin();
    const double* end = x.end();
    while (source!=end) {
        *(dest++) = *(source++) <= s;
    }
    return ret;
}
/*  Comparison operator */
Matrix operator==(const Matrix& x, double s ) {
    Matrix ret(x.nRows(), x.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = x.begin();
    const double* end = x.end();
    while (source!=end) {
        *(dest++) = *(source++) == s;
    }
    return ret;
}
/*  Comparison operator */
Matrix operator!=(const Matrix& x, double s) {
    Matrix ret(x.nRows(), x.nCols(), 0 );
    double* dest = ret.begin();
    const double* source = x.begin();
    const double* end = x.end();
    while (source!=end) {
        *(dest++) = *(source++) != s;
    }
    return ret;
}

/*  Comparison operator */
Matrix operator>(const Matrix& x, const Matrix& y ) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0);
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) > *(s2++);
    }
    return ret;
}
/*  Comparison operator */
Matrix operator>=(const Matrix& x,  const Matrix&  y ) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0);
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) >= *(s2++);
    }
    return ret;
}
/*  Comparison operator */
Matrix operator<(const Matrix& x,  const Matrix&  y ) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0);
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) < *(s2++);
    }
    return ret;
}
/*  Comparison operator */
Matrix operator<=(const Matrix& x,  const Matrix&  y ) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0);
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) <= *(s2++);
    }
    return ret;
}
/*  Comparison operator */
Matrix operator==(const Matrix& x,  const Matrix&  y ) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0);
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) == *(s2++);
    }
    return ret;
}
/*  Comparison operator */
Matrix operator!=(const Matrix& x,  const Matrix&  y) {
    ASSERT( x.nRows()==y.nRows() && x.nCols()==y.nCols());
    Matrix ret(x.nRows(), x.nCols(), 0);
    double* dest = ret.begin();
    const double* s1 = x.begin();
    const double* s2 = y.begin();
    const double* end = x.end();
    while (s1!=end) {
        *(dest++) = *(s1++) != *(s2++);
    }
    return ret;
}

/*  Matrix product */
Matrix operator*(const Matrix& a, const Matrix& b) {
	int m = a.nRows();
	int r = a.nCols();
	int n = b.nCols();
	ASSERT(b.nRows() == r);
	Matrix ret(m, n);
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < r; k++) {
				ret(i, j) += a(i, k)*b(k, j);
			}
		}
	}
	return ret;
}



////////////////////////////////
//
//   TESTS
//
////////////////////////////////

static void testBasics() {
    Matrix m(3,8);
    ASSERT( m.nRows()==3 );
    ASSERT( m.nCols()==8 );
    for (int i=0; i<m.nRows(); i++) {
        for (int j=0; j<m.nCols(); j++) {
            ASSERT( m(i,j)==0.0 );
            ASSERT( m.get(i,j)==m(i,j) );

            m(i,j)=i+j;
            ASSERT( m.get(i,j)==i+j );

            m.set(i,j,0.0);
            ASSERT( m(i,j)==0.0 );
        }
    }
}

static void testCopy() {
    Matrix m(3,8);
    for (int i=0; i<m.nRows(); i++) {
        for (int j=0; j<m.nCols(); j++) {
           m(i,j)=i+j;
        }
    }
    // verify the copy constructor
    Matrix n(m);
    for (int i=0; i<m.nRows(); i++) {
        for (int j=0; j<m.nCols(); j++) {
            ASSERT( n(i,j)==i+j );
            n(i,j)=0;
        }
    }
    // verify the assignment operator
    m=n;
    for (int i=0; i<m.nRows(); i++) {
        for (int j=0; j<m.nCols(); j++) {
            ASSERT( m(i,j)==0 );
        }
    };
}

static void testAdditionAndSubtrationOperators() {
    Matrix z=zeros(3,2);
    Matrix m=ones(3,2);
    Matrix n=ones(3,2);

	m.assertEquals(n, 0.001);
    (1+m).assertEquals(2*n,0.001);
    (1+m).assertEquals(n*2,0.001);
    (m+1).assertEquals(n*2,0.001);
    (m+m+n).assertEquals(n*3,0.001);
    (m-1).assertEquals(z,0.001);
    (1-m).assertEquals(z,0.001);
    (m-m).assertEquals(z,0.001);
}

static void testComparisonOperators() {
    Matrix falseM=zeros(3,2);
	Matrix trueM=ones(3,2);
	Matrix three=3*ones(3,2);
    Matrix four=4*ones(3,2);

	trueM.assertEquals( three<four, 0.001);
	falseM.assertEquals( four<three, 0.001);
	falseM.assertEquals( three<three, 0.001);

	trueM.assertEquals( three<=four, 0.001);
	falseM.assertEquals( four<=three, 0.001);
	trueM.assertEquals( three<=three, 0.001);

	falseM.assertEquals( three>four, 0.001);
	trueM.assertEquals( four>three, 0.001);
	falseM.assertEquals( three>three, 0.001);

	falseM.assertEquals( three>=four, 0.001);
	trueM.assertEquals( four>=three, 0.001);
	trueM.assertEquals( three>=three, 0.001);

	falseM.assertEquals( three==four, 0.001);
	falseM.assertEquals( four==three, 0.001);
	trueM.assertEquals( three==three, 0.001);

	trueM.assertEquals( three!=four, 0.001);
	trueM.assertEquals( four!=three, 0.001);
	falseM.assertEquals( three!=three, 0.001);

	trueM.assertEquals( 3<four, 0.001);
	falseM.assertEquals( 4<three, 0.001);
	falseM.assertEquals( 3<three, 0.001);

	trueM.assertEquals( 3<=four, 0.001);
	falseM.assertEquals( 4<=three, 0.001);
	trueM.assertEquals( 3<=three, 0.001);

	falseM.assertEquals( 3>four, 0.001);
	trueM.assertEquals( 4>three, 0.001);
	falseM.assertEquals( 3>three, 0.001);

	falseM.assertEquals( 3>=four, 0.001);
	trueM.assertEquals( 4>=three, 0.001);
	trueM.assertEquals( 3>=three, 0.001);

	falseM.assertEquals( 3==four, 0.001);
	falseM.assertEquals( 4==three, 0.001);
	trueM.assertEquals( 3==three, 0.001);

	trueM.assertEquals( 3!=four, 0.001);
	trueM.assertEquals( 4!=three, 0.001);
	falseM.assertEquals( 3!=three, 0.001);

	trueM.assertEquals( three<4, 0.001);
	falseM.assertEquals( four<3, 0.001);
	falseM.assertEquals( three<3, 0.001);

	trueM.assertEquals( three<=4, 0.001);
	falseM.assertEquals( four<=3, 0.001);
	trueM.assertEquals( three<=3, 0.001);

	falseM.assertEquals( three>4, 0.001);
	trueM.assertEquals( four>3, 0.001);
	falseM.assertEquals( three>3, 0.001);

	falseM.assertEquals( three>=4, 0.001);
	trueM.assertEquals( four>=3, 0.001);
	trueM.assertEquals( three>=3, 0.001);

	falseM.assertEquals( three==4, 0.001);
	falseM.assertEquals( four==3, 0.001);
	trueM.assertEquals( three==3, 0.001);

	trueM.assertEquals( three!=4, 0.001);
	trueM.assertEquals( four!=3, 0.001);
	falseM.assertEquals( three!=3, 0.001);

}

static void testFunctions() {
	Matrix unity = ones(3,2);
	Matrix m = 3*ones(3,2);
	m.exp();
	m.assertEquals( exp(3.0)*unity, 0.001 );
	
	m = 3*ones(3,2);
	m.log();
	m.assertEquals( log(3.0)*unity, 0.001 );
	
	m = 3*ones(3,2);
	m.sqrt();
	m.assertEquals( sqrt(3.0)*unity, 0.001 );
	
	m = 3*ones(3,2);
	m.pow(0.5);
	m.assertEquals( pow(3.0,0.5)*unity, 0.001 );

	m = 3*ones(3,2);
	m.positivePart();
	m.assertEquals( 3*unity, 0.001 );

    m = -3*ones(3,2);
	m.positivePart();
	m.assertEquals( 0.0*unity, 0.001 );

    m = 3*ones(3,2);
	m.negativePart();
	m.assertEquals( 0.0*unity, 0.001 );

    m = -3*ones(3,2);
	m.negativePart();
	m.assertEquals( -3.0*unity, 0.001 );

}

static void testAssignmentOperators() {
	const Matrix u = ones(3,2);
	Matrix m = ones(3,2);
	
	m= ones(3,2);
	m+=1;
	m.assertEquals( u + u, 0.001 );

	m+=2*u;
	m.assertEquals( 4*u, 0.001 );

	m-=2*u;
	m.assertEquals( 2*u, 0.001 );

	m-=1;
	m.assertEquals( u, 0.001 );

	m*=8;
	m.assertEquals( 8*u, 0.001 );

	m.times(2*u);
	m.assertEquals( 16*u, 0.001 );

}

static void testRowVector() {
    Matrix m(1,10,1);
    for (int i=0; i<10; i++) {
        m(0,i)=i;
    }
    vector<double> row = m.rowVector();
    for (int i=0; i<10; i++) {
        ASSERT(row[i]==i);
    }
}

static void testColVector() {
    Matrix m(10,1);
    for (int i=0; i<10; i++) {
        m(i,0)=i;
    }
    vector<double> col = m.colVector();
    for (int i=0; i<10; i++) {
        ASSERT(col[i]==i);
    }
}

static void testSetRow() {
    Matrix row(1,10);
    for (int i=0; i<10; i++) {
        row(0,i)=i;
    }
    Matrix other(7,10);
    other.setRow(5,row,0);
    for (int i=0; i<10; i++) {
        ASSERT(other(5,i)==i);
    }
    Matrix rowVec = other.row( 5 );
    rowVec.assertEquals(row, 0.001);
}

static void testSetCol() {
    Matrix col(10,1);
    for (int i=0; i<10; i++) {
        col(i,0)=i;
    }
    Matrix other(10,7);
    other.setCol(5,col,0);
    for (int i=0; i<10; i++) {
        ASSERT(other(i,5)==i);
    }
    Matrix colVec = other.col( 5 );
    colVec.assertEquals(col, 0.001);
}

static void testTest() {
    Matrix tests(2,2);
    tests(0,0)=1;
    tests(0,1)=1;
    Matrix valueIfTrue = 3*ones(2,2);
    Matrix valueIfFalse = -3*ones(2,2);
    tests.test( valueIfTrue, valueIfFalse );
    ASSERT_APPROX_EQUAL( tests(0,0), 3.0, 0.0001);
    ASSERT_APPROX_EQUAL( tests(0,1), 3.0, 0.0001);
    ASSERT_APPROX_EQUAL( tests(1,0), -3.0, 0.0001);
    ASSERT_APPROX_EQUAL( tests(1,1), -3.0, 0.0001);
}

static void testReadFromString() {
    Matrix m("1,2,3;4,5,6");
    ASSERT(m.nRows()==2);
    ASSERT(m.nCols()==3);
    INFO("Matrix "<<m);
    double count = 1.0;
    for (int i=0; i<2; i++) {
        for (int j=0; j<3; j++) {
            ASSERT_APPROX_EQUAL( m(i,j), count, 0.001);
            count++;
        }
    }
}

static void testMatrixMultiplication() {
	Matrix a("1,2,3;4,5,6");
	Matrix b("1,2;3,4;5,6");
	Matrix product = a*b;
	Matrix expected("22,28;49,64");
	product.assertEquals(expected,0.001);
}

static void testUsageExamples() {
    // CONFIRM THAT THE usage examples in the notes
    // all compile
    if (true) {
        Matrix m1("1,2,3;4,5,6");
        Matrix m2("2,3,4;5,6,7");

        Matrix actual = m1 + m2;

        Matrix expected("3,5,7;9,11,13");
        expected.assertEquals( actual, 0.001 );
    }

    if (true) {
        Matrix test1("1,2;3,4");
        Matrix test2("3,3;3,3");
        Matrix expected("0.0,0.0;1.0,1.0");
        expected.assertEquals( test1>=test2, 0.001);

    }

    if (true) {
        Matrix m("1,2,3;4,5,6");
        ASSERT( m(1,2)==6 ); // read a value
        m(1,2)=0; // change the value
    }
}


void testMatrix() {
    TEST( testBasics );
    TEST( testRowVector );
    TEST( testColVector );
    TEST( testSetRow);
    TEST( testSetCol );
    TEST( testCopy);
    TEST( testAdditionAndSubtrationOperators );
	TEST( testComparisonOperators );
	TEST( testFunctions);
	TEST( testAssignmentOperators );
    TEST( testTest) ;
    TEST( testReadFromString );
    TEST( testUsageExamples );
	TEST( testMatrixMultiplication );
}
