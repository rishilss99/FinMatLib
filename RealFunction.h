#pragma once

/**
 *   A real function is something that can
 *   be evaluated at a real number to give
 *   a real number
 */
class RealFunction {
public:
    /*  A virtual destructor */
    virtual ~RealFunction() {};
    /*  This method is abstract, there is
        no definition */
    virtual double evaluate( double x ) = 0;

	double operator()(double x) {
		return evaluate(x);
	}
};