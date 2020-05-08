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

/* 全局声明 */
void initFdct (void);
void fDCTCalc (short *block);

static double c[8][8]; /* 变换系数 */

void initFdct()//利用这个函数得到DCT变换的变换核，并且赋给C矩阵
{
  int i, j;
  double s;

  for (i=0; i<8; i++)
  {
    s = (i==0) ? sqrt(0.125) : 0.5;

    for (j=0; j<8; j++)
      c[i][j] = s * cos((PI/8.0)*i*(j+0.5));
  }
}

void fDCTCalc(short *block)//dct变换函数,block是需要变换的矩阵的指针，得到的结果的指针
{
  int i, j, k;
  double s;
  double tmp[64];

  for (i=0; i<8; i++)
    for (j=0; j<8; j++)//对block的每一行做一维DCT变换，得到的结果放在tmp中
    {
      s = 0.0;

      for (k=0; k<8; k++)
        s += c[j][k] * block[8*i+k];

      tmp[8*i+j] = s;
    }

  for (j=0; j<8; j++)//对tmp的转制的行，即tmp的列做一维DCT变换，得到的结果依然存在block中
    for (i=0; i<8; i++)
    {
      s = 0.0;

      for (k=0; k<8; k++)
        s += c[i][k] * tmp[8*k+j];

      block[8*i+j] = (int)floor(s+0.499999);//将小数转化成整数，以便于量化
      /*
       这里需要说明一下，为什么采用0.499999而不采用0.5？
	   这是因为s的取值经常是以.5结尾的（至少在i，j等于
	   0或4的时候是这样），如果将阈值设为0.5将导致非常
	   高的算术误差，而如果采用0.499999则产生误差的概率
	   也就是s以.5和.500001之间的某个数值结尾的概率，显
	   然，这样的几率要小很多。
       */
    }
}
