#include "KnockoutOption.h"
#include "UpAndOutOption.h"

using namespace std;

ostream &operator<<(ostream &out, const KnockoutOption &option)
{   
    out << "*******************************************\n";
    out << "Option Type:" << objectType(option) << "\n";
    out << "Option Maturity: " << option.getMaturity() << "\n";
    out << "Option Barrier:" << option.getBarrier() << "\n";
    out << "Option Strike:" << option.getStrike() << "\n";
    out << "*******************************************\n";
    out << "\n";
    return out;
}
