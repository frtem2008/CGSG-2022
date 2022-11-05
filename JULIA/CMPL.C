/* AN2 */
#include <math.h>

#include "CMPL.h"

CMPL CmplSet( DBL Re, DBL Im )
{
  CMPL res = {Re, Im};

  return res;
}

CMPL CmplAdd( CMPL Z1, CMPL Z2 )
{
  return CmplSet(Z1.Re + Z2.Re, Z1.Im + Z2.Im);
}

CMPL CmplMultiply( CMPL Z1, CMPL Z2 )
{
  return CmplSet(Z1.Re * Z2.Re - Z1.Im * Z2.Im, Z1.Im * Z2.Re + Z1.Re * Z2.Im);
}

CMPL CmplSquare( CMPL Z )
{
  return CmplMultiply(Z, Z);
}

DBL CmplMod( CMPL Z )
{
  return sqrt(Z.Re * Z.Re + Z.Im * Z.Im);
}

DBL CmplModSquared( CMPL Z )
{
  return (Z.Re * Z.Re + Z.Im * Z.Im);
}

INT Julia( CMPL Z, CMPL C )
{
  INT n = 0;
  
  while (n < 255 && CmplModSquared(Z) <= 4)
	Z = CmplAdd(CmplSquare(Z), C), n++;

  return n;
}

