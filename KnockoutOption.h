#pragma once

#include "ContinuousTimeOptionBase.h"

/**
 *   An option with a barrier
 */
class KnockoutOption : public ContinuousTimeOptionBase {
public:
    virtual ~KnockoutOption() {}

    double getBarrier() const {
        return barrier;
    }

    void setBarrier(double barrier) {
        this->barrier=barrier;
    }

    bool isPathDependent() const {
        return true;
    }
private:
    double barrier;
};

std::ostream &operator<<(std::ostream &out, const KnockoutOption &option);

void testKnockoutDebug();