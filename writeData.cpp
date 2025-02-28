/* 比特输出*/


#include <cstdio>

#include "timeSettings.h"
extern FILE *outfile;


static unsigned char outbfr;
static int outcnt;
static int bytecnt;

/* 初始化缓冲区，在初次写入或填充前调用 */
void writeInit()
{
  outcnt = 8;
  bytecnt = 0;
}

/* 从缓冲区val中向文件写入最多不超过32b的数据。 */
void writeData(int val,int n)
{
  int i;
  unsigned int mask;

  mask = 1 << (n-1); /* 选择第一个比特，即最左侧的比特 */

  for (i=0; i<n; i++)
  {
    outbfr <<= 1;

    if (val & mask)
      outbfr|= 1;

    mask >>= 1; /* 选择下一个比特 */
    outcnt--;

    if (outcnt==0) /* 单字节缓冲区满*/
    {
      putc(outbfr,outfile);
      outcnt = 8;
      bytecnt++;
    }
  }
}

/* 满足字节边界而填充0比特 */
void dataAlign()
{
  if (outcnt!=8)
    writeData(0,outcnt);
}

/* 返回生成的全部比特计数 */
int dataCount()
{
  return 8*bytecnt + (8-outcnt);
}
