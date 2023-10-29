#pragma once

#include "KnockoutOption.h"

class UpAndOutOption : public KnockoutOption {
public:
    Matrix payoff(
        const Matrix& prices ) const;
};


void testUpAndOutOption();