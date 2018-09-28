#include "fraction.hh"
#include "QDebug"

Fraction::Fraction(int numerator, int denominator)
{
    Q_ASSERT(denominator != 0);
    Fraction::numerator_ = numerator;
    Fraction::denominator_ = denominator;
}

double Fraction::giveAsFloating() const
{
    double fraction = double(Fraction::numerator_) / double(Fraction::denominator_);
    return fraction;
}
