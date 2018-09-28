#ifndef FRACTION_HH
#define FRACTION_HH

class Fraction
{
public:
    /*!
   * \brief Constructor for Fraction
   * \param numerator
   * \param denominator
   * \pre denominator != 0
   */
  Fraction(int numerator, int denominator);

  /*!
 * \brief Destructor
 * \pre -
 */
  ~Fraction();

  /*!
   * \brief giveNumerator
   * \pre -
   * \post should not change any value
   * \return numerator_
   */
  int giveNumerator() const;

  /*!
   * \brief givedenominator
   * \pre -
   * \post should not change any value
   * \return denominator_
   */
  int givedenominator() const;

  /*!
   * \brief giveAsFloating
   * \pre -
   * \post should not change any value
   * \return float(nominator_)/float(denominator_)
   */
  double giveAsFloating() const;

  Fraction& operator+=(Fraction f);
  Fraction& operator/=(Fraction f);
// ...

private:
  int numerator_;
  int denominator_;
};

#endif // Fraction_HH
