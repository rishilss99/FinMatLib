#pragma once

#include "stdafx.h"

const double PI = 3.1415926535897932384626433832795028842;

// forward declarations, allows references
// to be used between the different classes
class Shape;
class Circle;
class PolarPoint;
class CartesianPoint;
class Rectangle;

/**
 *   Abstract class shape
 */
class Shape {
public:
    /*  Does the point lie in the shape */
    virtual bool contains( const CartesianPoint& point )
        const = 0;
    /*  A rectangle bounding the shape */
    virtual Rectangle boundingRectangle() const = 0;
    /*  By default the area is computed by Monte Carlo */
    virtual double area() const;

    static std::shared_ptr<Shape> newCircle(
            const CartesianPoint& center,
            double radius );

    static std::shared_ptr<Shape> newHyperCircle();
};

class CartesianPoint {
public:
    double getX() const {
        return x;
    }
    double getY() const {
        return y;
    }
    void setX( double x ) {
        this->x=x;
    }
    void setY( double y ) {
        this->y=y;
    }

    PolarPoint toPolar() const;
    double distanceTo( const CartesianPoint& x ) const;
    double distanceTo( const PolarPoint& x ) const;

    CartesianPoint() : x(0.0), y(0.0) {}
    CartesianPoint( double x, double y ) : x(x), y(y) {}
private:
    double x;
    double y;
};

class PolarPoint {
public:
    double getR() const {
        return r;
    }
    double getTheta() const {
        return theta;
    }

    void setR( double r ) {
        this->r = r;
    }

    void setTheta( double theta ) {
        this->theta = theta;
    }
    CartesianPoint toCartesian() const;
    PolarPoint() : r(0.0), theta(0.0) {}
private:
    double r;
    double theta;
};

class Circle : public Shape {
public:

    double area() const ;
    double circumference() const ;
    bool contains( const CartesianPoint& point ) const;
    Rectangle boundingRectangle() const;

    Circle() : center(), radius(0.0) {}

    double getRadius() const {
        return radius;
    }
    void setRadius( double radius ) {
        this->radius = radius;
    }
    CartesianPoint getCenter() const {
        return center;
    }
    void setCenter( const CartesianPoint& center ) {
        this->center = center;
    }

private:
    CartesianPoint center;
    double radius;
};

class Rectangle : public Shape {
public:
    Rectangle() : top(0), bottom(0), left(0), right(0) {}
    Rectangle(double top, double bottom, double left, double right )
        : top(top), bottom(bottom), left(left), right(right) {}
    double area() const {
        return getWidth()*getHeight();
    }
    Rectangle boundingRectangle() const {
        return *this;
    }

    bool contains( const CartesianPoint& p ) const;

    double getTop() const {
        return top;
    }
    void setTop( double top ) {
        this->top = top;
    }
    double getBottom() const {
        return bottom;
    }
    void setBottom( double bottom ) {
        this->bottom = bottom;
    }
    double getLeft() const {
        return left;
    }
    void setLeft( double left ) {
        this->left = left;
    }
    double getRight() const {
        return right;
    }
    void setRight( double right ) {
        this->right = right;
    }
    double getWidth() const {
        return right-left;
    }
    double getHeight() const {
        return top-bottom;
    }

private:
    double top;
    double bottom;
    double left;
    double right;
};

class HyperCircle : public Shape {
public:
    Rectangle boundingRectangle() const {
        return standardRectangle;
    }
    bool contains( const CartesianPoint& p ) const {
        return pow(p.getX(),4)+pow(p.getY(),4)<=1;
    }
private:
    static Rectangle standardRectangle;
};


/**
 *  Test suite
 */
void testGeometry();