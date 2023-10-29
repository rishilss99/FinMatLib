#include "PathIndependentOption.h"

std::ostream &operator<<(std::ostream &out, const PathIndependentOption &option)
{
    out << "*******************************************\n";
    out << "Option Type:" << objectType(option) << "\n";
    out << "Option Maturity: " << option.getMaturity() << "\n";
    out << "Option Strike:" << option.getStrike() << "\n";
    out << "*******************************************\n";
    out << "\n";
    return out;
}