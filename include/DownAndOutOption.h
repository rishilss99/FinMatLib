#pragma once

#include "KnockoutOption.h"

class DownAndOutOption : public KnockoutOption {
public:
    Matrix payoff(
        const Matrix& prices ) const;
};


void testDownAndOutOption();