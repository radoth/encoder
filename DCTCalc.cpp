/* 前向离散余弦变换，双精度*/



#include <cmath>

#include "timeSettings.h"

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

static double c[8][8];

void initFdct()
{
  double value;
  for (int i=0; i<8; i++)
  {
    value = (i==0) ? sqrt(0.125) : 0.5;

    for (int j=0; j<8; j++)
      c[i][j] = value * cos((PI/8.0)*i*(j+0.5));
  }
}

void fDCTCalc(short *current)
{
  double value;
  double data[64];

  for (int i=0; i<8; i++)
    for (int j=0; j<8; j++)
    {
      value = 0.0;

      for (int k=0; k<8; k++)
        value += c[j][k] * current[8*i+k];

      data[8*i+j] = value;
    }

  for (int j=0; j<8; j++)
    for (int i=0; i<8; i++)
    {
      value = 0.0;

      for (int k=0; k<8; k++)
        value += c[i][k] * data[8*k+j];

      current[8*i+j] = (int)floor(value+0.5);
    }
}
