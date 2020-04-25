/* fdctref.c, ǰ����ɢ���ұ任��˫����*/



#include <math.h>

#include "config.h"

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

/* ȫ������ */
void init_fdct _ANSI_ARGS_((void));
void fdct _ANSI_ARGS_((short *block));

static double c[8][8]; /* �任ϵ�� */

void init_fdct()
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

void fdct(block)
short *block;
{
  int i, j, k;
  double s;
  double tmp[64];

  for (i=0; i<8; i++)
    for (j=0; j<8; j++)
    {
      s = 0.0;

      for (k=0; k<8; k++)
        s += c[j][k] * block[8*i+k];

      tmp[8*i+j] = s;
    }

  for (j=0; j<8; j++)
    for (i=0; i<8; i++)
    {
      s = 0.0;

      for (k=0; k<8; k++)
        s += c[i][k] * tmp[8*k+j];

      block[8*i+j] = (int)floor(s+0.499999);
      /*
       ������Ҫ˵��һ�£�Ϊʲô����0.499999��������0.5��
	   ������Ϊs��ȡֵ��������.5��β�ģ�������i��j����
	   0��4��ʱ�������������������ֵ��Ϊ0.5�����·ǳ�
	   �ߵ����������������0.499999��������ĸ���
	   Ҳ����s��.5��.500001֮���ĳ����ֵ��β�ĸ��ʣ���
	   Ȼ�������ļ���ҪС�ܶࡣ
       */
    }
}
