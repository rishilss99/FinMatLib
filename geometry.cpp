#include "geometry.h"
#include "stdafx.h"
#include "testing.h"
#include "matlib.h"

using namespace std;

////////////////////////////////
//    Shape implementation
////////////////////////////////
double Shape::area() const {
    Rectangle r = boundingRectangle();
    int count = 0;
    int nPoints = 10000;
    Matrix u = randuniform( nPoints, 2 );
    for (int i=0; i<nPoints; i++) {
        double x = u(i,0) * r.getWidth() + r.getLeft();
        double y = u(i,1) * r.getHeight() + r.getBottom();
        CartesianPoint p(x,y);
        if (contains(p)) {
            count++;
        }
    }
    return ((double)count)/nPoints * r.area();
}

shared_ptr<Shape> Shape::newCircle( const CartesianPoint& cartesianPoint,
    double radius ) {
    shared_ptr<Circle> ret = make_shared<Circle>();
    ret->setCenter( cartesianPoint );
    ret->setRadius( radius );
    return ret;
}

shared_ptr<Shape> Shape::newHyperCircle() {
    shared_ptr<HyperCircle> ret= make_shared<HyperCircle>();
    return ret;
}



////////////////////////////////
//    Circle implementation
////////////////////////////////

/**
 *   Computes the area of a circle
 */
double Circle::area() const {
	return PI*radius*radius;
}

/**
 *  Computes the circumference of a circle
 */
double Circle::circumference() const {
	return 2*PI*radius;
}

bool Circle::contains( const CartesianPoint& x ) const {
    return x.distanceTo( center )<=radius;
}

Rectangle Circle::boundingRectangle() const {
    Rectangle r;
    r.setTop( center.getY()+radius );
    r.setBottom( center.getY()-radius );
    r.setLeft( center.getX()-radius );
    r.setRight( center.getX()+radius );
    return r;
}

////////////////////////////////
//    CartesianPoint implementation
////////////////////////////////


/**
 *  Computes the distance between two cartesian points
 */
double CartesianPoint::distanceTo( const CartesianPoint& other ) const {
    double dx = other.x - x;
    double dy = other.y - y;
    return sqrt( dx*dx + dy*dy );
}

/**
 *  Computes the distance between this and a polar point
 */
double CartesianPoint::distanceTo( const PolarPoint& other ) const {
    return distanceTo( other.toCartesian());
}



/**
 *  Converts polar coordinates to cartesian. Note, this changes x and y.
 */
CartesianPoint PolarPoint::toCartesian() const {
    CartesianPoint c;
	c.setX( r*cos( theta ));
	c.setY( r*sin( theta ));
    return c;
}


////////////////////////////////
//    PolarPoint implementation
////////////////////////////////

/**
 *   Converts Cartesian coordinates to polar coordinates. Note this
 *   changes r and theta.
 */
PolarPoint CartesianPoint::toPolar() const {
    PolarPoint p;
	// Not as easy as it looks to do this
	p.setR( sqrt( x*x + y*y));
	if (y==0.0) {
		if (x>=0.0) {
			p.setTheta( 0.0);
		} else if (x<0.0) {
			p.setTheta( -PI);
		}
	} else {
		p.setTheta( acos( x/p.getR()));
		if (y<0) {
			p.setTheta( -p.getTheta());
		}
	}
    return p;
}

////////////////////////////////
//    Rectangle implementation
////////////////////////////////

bool Rectangle::contains(const CartesianPoint& p) const {
    double x = p.getX();
    double y = p.getY();
    return x>=left && x<=right && y<=top && y>=bottom;
}

////////////////////////////////
//   HyperCircle implementation
////////////////////////////////

Rectangle HyperCircle::standardRectangle(1,-1,-1,1);

/////////////////////////////////////////////////
//
//  Geometry tests
//
/////////////////////////////////////////////////


static void testAreaOfCircle() {
    Circle c;
    c.setRadius( 4 );
	ASSERT_APPROX_EQUAL( c.area(), 16*PI, 0.01 );
}

static void testCircumferenceOfCircle() {
    Circle c;
    c.setRadius( 2 );
	ASSERT_APPROX_EQUAL( c.circumference(), 4*PI, 0.01 );
}

static void testPolarToCartesian() {
    PolarPoint p;
    p.setR( 2.0 );
    p.setTheta( PI/2 );
	CartesianPoint c = p.toCartesian();
	ASSERT_APPROX_EQUAL( c.getX(),0.0,0.001 );
	ASSERT_APPROX_EQUAL( c.getY(),2.0,0.001 );
}

static void testCartesianToPolar() {
	// the logic of cartesian to polar is quite complex so we have more tests
	vector<double> angles;
	angles.push_back( 0.0 );
	angles.push_back( PI/4 );
	angles.push_back( PI/2 );
	angles.push_back( 7*PI/8 );
	angles.push_back( PI);
	angles.push_back( -PI/4 );
	angles.push_back( -PI/2 );
	angles.push_back( -7*PI/8 );

	int nAngles = angles.size();
	for (int i=0; i<nAngles; i++) {
        PolarPoint p;
		p.setR( 2.0 );
		p.setTheta(angles[i]);
		CartesianPoint c =p.toCartesian();
		PolarPoint p2 = c.toPolar();
		ASSERT_APPROX_EQUAL( p.getR(),p2.getR(),0.001 );
		ASSERT_APPROX_EQUAL( p.getTheta(),p2.getTheta(),0.001 );
	}
}

static void testDistanceTo() {
    CartesianPoint p1;
    p1.setX( 1 );
    p1.setY( 1 );
    CartesianPoint p2;
    p2.setX( 4 );
    p2.setY( 5 );
    double d = p1.distanceTo( p2 );
    ASSERT_APPROX_EQUAL( d, 5.0, 0.0001);    
}

static void testDistanceToPolar() {
    CartesianPoint p1;
    p1.setX( 1 );
    p1.setY( 1 );
    CartesianPoint p2;
    p2.setX( 4 );
    p2.setY( 5 );
    double d = p1.distanceTo( p2.toPolar() );
    ASSERT_APPROX_EQUAL( d, 5.0, 0.0001);    
}

static void testRectangle() {
    Rectangle r;
    r.setTop( 1 );
    r.setBottom(-1);
    r.setLeft( -2 );
    r.setRight(2);
    ASSERT_APPROX_EQUAL( r.area(), 8.0, 0.0001);
}

static void testHyperCircle() {
    HyperCircle h;
    CartesianPoint p(0.5,0);
    ASSERT( h.contains(p));
    p = CartesianPoint(0,0.5);
    ASSERT( h.contains(p));
    Circle c;
    c.setRadius(1);
    Rectangle r = h.boundingRectangle();
    ASSERT( h.area()>c.area() );
    ASSERT( h.area()<r.area() );
}




void testGeometry() {
	TEST( testAreaOfCircle );
	TEST( testCircumferenceOfCircle );
	TEST( testPolarToCartesian );
	TEST( testCartesianToPolar );
    TEST( testDistanceTo );
    TEST( testDistanceToPolar );
    TEST( testRectangle );
    TEST( testHyperCircle);
}