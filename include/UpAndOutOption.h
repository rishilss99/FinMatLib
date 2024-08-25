#pragma once

#include "KnockoutOption.h"

class UpAndOutOption : public KnockoutOption {
public:
    Matrix payoff(
        const Matrix& prices ) const;
};

typedef std::shared_ptr<UpAndOutOption> SPUpAndOutOption;
typedef std::shared_ptr<const UpAndOutOption> SPCUpAndOutOption;


void testUpAndOutOption();