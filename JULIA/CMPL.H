/* AN2 */

typedef int INT;
typedef double DBL;

typedef struct tagCMPL
{
  DBL Re, Im;
} CMPL;

CMPL CmplSet( DBL Re, DBL Im );
INT Julia( CMPL Z, CMPL C );